#include "ModbusCVD.h"
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

// 静的メンバーの定義
std::mutex ModbusCVD::modbusMutex;

// 既存のコンストラクタ
ModbusCVD::ModbusCVD(const std::string& device, int baudrate, int slaveId)
    : slaveId(slaveId), rateAcc(30000), rateDec(30000), OpeCurrent(1000), ownsContext(true) {
    ctx = modbus_new_rtu(device.c_str(), baudrate, 'E', 8, 1);
    if (ctx == nullptr || modbus_connect(ctx) == -1) {
        std::cerr << "Connection failed: " << modbus_strerror(errno) << std::endl;
        exit(1);
    }
    initialize();
}

// 新しいコンストラクタ：共有コンテキスト用
ModbusCVD::ModbusCVD(modbus_t* sharedCtx, int slaveId)
    : ctx(sharedCtx), slaveId(slaveId), rateAcc(30000), rateDec(30000), OpeCurrent(1000), ownsContext(false) {
    if (ctx == nullptr) {
        std::cerr << "Shared context is null" << std::endl;
        exit(1);
    }
    initialize();
}

ModbusCVD::~ModbusCVD() {
    if (ctx && ownsContext) {
        modbus_close(ctx);
        modbus_free(ctx);
    }
}

void ModbusCVD::initialize() {
    std::lock_guard<std::mutex> lock(modbusMutex);

    // スレーブIDを設定
    if (modbus_set_slave(ctx, slaveId) == -1) {
        std::cerr << "Failed to set slave ID " << slaveId << ": " << modbus_strerror(errno) << std::endl;
        driveCmd = 0;
        return;
    }

    // 少し待機してからレジスタ読み取り（デバイスの応答待ち）
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    uint16_t reg;
    if (modbus_read_registers(ctx, 0x007d, 1, &reg) != -1) {
        driveCmd = reg;
        std::cout << "Slave " << slaveId << " initialized, driveCmd = 0x" << std::hex << driveCmd << std::dec << std::endl;
    }
    else {
        std::cerr << "Failed to read drive command register for slave " << slaveId << ": " << modbus_strerror(errno) << std::endl;
        driveCmd = 0;
    }
}

bool ModbusCVD::writeRegs(int addr, const uint16_t* data, int len) {
    std::lock_guard<std::mutex> lock(modbusMutex);

    // スレーブIDを設定
    if (modbus_set_slave(ctx, slaveId) == -1) {
        std::cerr << "Failed to set slave ID " << slaveId << ": " << modbus_strerror(errno) << std::endl;
        return false;
    }

    // 少し待機（前の通信との干渉を避けるため）
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    int result = modbus_write_registers(ctx, addr, len, data);
    if (result == -1) {
        std::cerr << "Write failed for slave " << slaveId << " at address 0x" << std::hex << addr
            << ": " << modbus_strerror(errno) << std::dec << std::endl;
        return false;
    }

    return true;
}

bool ModbusCVD::writeRegs32(int addr, const int32_t* data, int len) {
    uint16_t buf[64];
    for (int i = 0; i < len; ++i) {
        buf[i * 2] = (data[i] >> 16) & 0xFFFF;
        buf[i * 2 + 1] = data[i] & 0xFFFF;
    }
    return writeRegs(addr, buf, len * 2);
}

bool ModbusCVD::resetAlarm() {
    uint16_t val[2] = { 0, 1 };
    if (!writeRegs(0x0180, val, 2)) return false;
    val[1] = 0;
    if (!writeRegs(0x0180, val, 2)) return false;
    val[0] = driveCmd;
    return writeRegs(0x007d, val, 1);
}

bool ModbusCVD::free(bool sw) {
    uint16_t val = sw ? (driveCmd | 0x0040) : (driveCmd & 0xFFBF);
    return writeRegs(0x007d, &val, 1);
}

bool ModbusCVD::writeParamAcc(int time, int speed) {
    int acc = (1000 * speed) / time;
    if (acc <= 0 || acc > 1'000'000'000) return false;
    rateAcc = acc;
    return true;
}

bool ModbusCVD::writeParamDec(int time, int speed) {
    int dec = (1000 * speed) / time;
    if (dec <= 0 || dec > 1'000'000'000) return false;
    rateDec = dec;
    return true;
}

bool ModbusCVD::writeParamCurrent(int current) {
    if (current < 0 || current > 1000) return false;
    OpeCurrent = current;
    return true;
}

bool ModbusCVD::startPosition(int position, int speed, int OpeType) {
    if (OpeType < 1 || OpeType > 2) return false;

    int32_t data[] = {
        0,          // OpeDataNo
        OpeType,    // 1=絶対位置, 2=相対位置
        position,
        speed,
        rateAcc,
        rateDec,
        OpeCurrent,
        1,          // Trigger
        0           // Memory
    };

    // Trigger = 1 から 0 へのパルス送信
    if (!writeRegs32(0x0058, data, 9)) return false;
    data[7] = 0;
    if (!writeRegs32(0x0058, data, 9)) return false;

    return true;
}

bool ModbusCVD::readHoldingRegisters(int address, uint16_t* value, int num_registers) {
    std::lock_guard<std::mutex> lock(modbusMutex); // 排他制御

    if (modbus_set_slave(ctx, slaveId) == -1) {
        std::cerr << "Failed to set slave ID " << slaveId << ": " << modbus_strerror(errno) << std::endl;
        return false;
    }

    // 短い待機時間（前の通信との干渉を避けるため）
    // ポーリングが高速な場合、通信安定化のために必要
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    int result = modbus_read_registers(ctx, address, num_registers, value);
    if (result == -1) {
        // エラー発生時
        std::cerr << "Read (Holding Registers) failed for slave " << slaveId << " at address 0x" << std::hex << address
            << ": " << modbus_strerror(errno) << std::dec << " (Error: " << errno << ")" << std::endl;
        // 値を0にリセットして、不正な値が伝播しないようにする
        if (value != nullptr) {
            for (int i = 0; i < num_registers; ++i) {
                value[i] = 0;
            }
        }
        return false;
    }
    else if (result == num_registers) {
        // 成功時
        return true;
    }
    else {
        // 読み込み数は合っているが、なぜかnum_registersと一致しない場合（稀）
        std::cerr << "DEBUG: Mismatch in read count. Expected " << num_registers << ", got " << result << " for slave " << slaveId << std::endl;
        return false;
    }
}

bool ModbusCVD::initializeAndStart(int position, int speed, int mode, const std::string& motorName) {
    std::cout << motorName << " - Resetting alarm..." << std::endl;
    if (!resetAlarm()) {
        std::cerr << motorName << " - Alarm reset failed." << std::endl;
        return false;
    }

    std::cout << motorName << " - Freeing motor..." << std::endl;
    if (!free(false)) {
        std::cerr << motorName << " - Free failed." << std::endl;
        return false;
    }

    std::cout << motorName << " - Setting acceleration/deceleration parameters..." << std::endl;
    if (!writeParamAcc(100, 200)) return false; 
    if (!writeParamDec(100, 200)) return false; 
    if (!writeParamCurrent(1000)) return false; 

    std::cout << motorName << " - Starting position control (Position: " << position << ", Speed: " << speed << ", Mode: " << mode << ")..." << std::endl;
    if (startPosition(position, speed, mode)) {
        std::cout << motorName << " command sent successfully. Waiting for completion..." << std::endl;

        // モーター動作完了の待機ロジック
        const int MAX_WAIT_SECONDS = 60;
        auto start_time = std::chrono::high_resolution_clock::now();

        bool is_motor_completed = false;
        uint16_t driver_output_status = 0; // ドライバ出力状態レジスタの値
        uint16_t current_alarm = 0;        // 現在のアラームレジスタの値

        const int DRIVER_OUTPUT_REGISTER_ADDRESS = 0x007F; // ドライバ出力状態 (下位) レジスタ
        const int ALARM_REGISTER_ADDRESS = 0x0080;         // 現在のアラームレジスタ

        // マニュアルP95に基づいたビットマスク
        // READY出力: 0x007F (下位) レジスタの Bit5 に割り付け
        const uint16_t READY_BIT_MASK = (1 << 5); 

        // MOVE出力: 0x007F (下位) レジスタの Bit13 に割り付け
        // マニュアルの図から、16ビットワードのBit13がMOVEに対応。
        const uint16_t MOVE_BIT_MASK = (1 << 13); 

        while (!is_motor_completed) {
            long long elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - start_time
                ).count();

            // ドライバ出力状態レジスタから値を読み込む
            bool read_output_ok = readHoldingRegisters(DRIVER_OUTPUT_REGISTER_ADDRESS, &driver_output_status, 1);
            if (read_output_ok) {
                // 読み込んだ値から、MOVEとREADYのビットを抽出
                uint16_t move_status = (driver_output_status & MOVE_BIT_MASK) ? 1 : 0;
                uint16_t ready_status = (driver_output_status & READY_BIT_MASK) ? 1 : 0;
            }
            else {
                std::cerr << motorName << " - (Err: Driver Output Read Failed)";
                // 通信エラーの場合は、一時的にMOVE/READYを不明な状態にする（例: 0）
                // または前回の値を保持する
                // ここでは、エラー発生時に現在のループを継続するが、ステータスは更新されないまま
                // ループ続行のため、is_motor_completedはfalseのまま
            }

            // 現在のアラームの読み込み
            bool read_alarm_ok = readHoldingRegisters(ALARM_REGISTER_ADDRESS, &current_alarm, 1);
            if (read_alarm_ok) {
                if (current_alarm != 0) {
                    std::cerr << " - !!! ALARM DETECTED !!! Code: 0x" << std::hex << current_alarm << std::dec << std::endl;
                    // アラームが発生したら、ここでモーターを停止するなどの適切なエラー処理を行う
                    // 例: resetAlarm()を呼んでからリトライする、または即座に失敗として終了する
                    return false; // アラームが出たら即座に失敗として終了
                }
            }
            else {
                std::cerr << " (Err: ALARM Read Failed)";
            }

            // 完了条件の確認
            // 両方の読み取りが成功し、かつステータスが完了条件を満たしている場合
            if (read_output_ok && ((driver_output_status & MOVE_BIT_MASK) == 0) && ((driver_output_status & READY_BIT_MASK) != 0)) {
                // MOVEビットが0（停止中）かつREADYビットが1（準備完了）
                is_motor_completed = true;
            }

            // タイムアウトチェック
            if (elapsed_seconds >= MAX_WAIT_SECONDS) {
                std::cerr << motorName << " - Motor operation timed out after " << MAX_WAIT_SECONDS << " seconds." << std::endl;
                // タイムアウト時には、現在のモーターの状態を詳しく出力する
                std::cerr << "  (Last known output status: 0x" << std::hex << driver_output_status
                    << ", MOVE bit: " << ((driver_output_status & MOVE_BIT_MASK) ? 1 : 0)
                    << ", READY bit: " << ((driver_output_status & READY_BIT_MASK) ? 1 : 0)
                    << ", Alarm: 0x" << current_alarm << std::dec << ")" << std::endl;
                return false; // タイムアウトは失敗とみなす
            }

            // ポーリング間隔を調整し、コンソール出力と通信負荷のバランスを取る
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        std::cout << motorName << " operation completed successfully." << std::endl;
        return true;
    }
    else {
        std::cerr << motorName << " start failed." << std::endl;
        return false;
    }
}

// 既存のstopMotor関数は減速停止として残すか、削除するか検討
// 今回は新しい関数として追加し、用途を明確に分ける
//bool ModbusCVD::stopMotor() { // 減速停止用 (以前のSTOP入力のまま)
//    std::lock_guard<std::mutex> lock(modbusMutex);
//    modbus_set_slave(ctx, slaveId);
//
//    // ドライバ入力コマンド (0x007D) のビット1 (STOP) をONにする
//    uint16_t driver_input_command_reg = (driveCmd | (1 << 1)); // driveCmdの既存ビットを維持し、STOPビットをON
//    if (!writeRegs(0x007D, &driver_input_command_reg, 1)) {
//        std::cerr << "Failed to send STOP command for slave " << slaveId << std::endl;
//        return false;
//    }
//
//    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 短いパルスとして送信するため、待機
//
//    // ドライバ入力コマンドのビット1 (STOP) をOFFに戻す
//    driver_input_command_reg = (driveCmd & ~(1 << 1)); // driveCmdの既存ビットを維持し、STOPビットをOFF
//    if (!writeRegs(0x007D, &driver_input_command_reg, 1)) {
//        std::cerr << "Failed to reset STOP command for slave " << slaveId << std::endl;
//        return false;
//    }
//    std::cout << "Slave " << slaveId << " received STOP command." << std::endl;
//    return true;
//}

// モーターを瞬間停止させる関数 (FREE入力を使用)
bool ModbusCVD::stopMotor() {
    std::cout << "Slave " << slaveId << " - Sending instant stop (FREE) command." << std::endl;
    // FREE入力をONにする (0x007dのビット6を立てる)
    if (!free(true)) {
        std::cerr << "Failed to send instant stop (FREE) command for slave " << slaveId << std::endl;
        return false;
    }
    std::cout << "Slave " << slaveId << " received instant stop command." << std::endl;
    return true;
}

//位置決め運転のpositionを膨大な数にすることで疑似的な連続運転を実現
bool ModbusCVD::moveMotor(int speed, int direction, const std::string& motorName) {
    if (direction < 1 || direction > 2) return false;

    std::cout << motorName << " - Resetting alarm..." << std::endl;
    if (!resetAlarm()) {
        std::cerr << motorName << " - Alarm reset failed." << std::endl;
        return false;
    }

    std::cout << motorName << " - Freeing motor..." << std::endl;
    if (!free(false)) {
        std::cerr << motorName << " - Free failed." << std::endl;
        return false;
    }

    std::cout << motorName << " - Setting acceleration/deceleration parameters..." << std::endl;
    if (!writeParamAcc(100, 200)) return false;
    if (!writeParamDec(100, 200)) return false;
    if (!writeParamCurrent(1000)) return false;

    int target_position = 2000000000;
    if (direction == 1) {
        target_position = target_position;
    }
    else if (direction == 2) {
        target_position = -target_position;
    }

    std::cout << motorName << " - Starting position control (Speed: " << speed << ")..." << std::endl;
    if (startPosition(target_position, speed, 2)) {
        std::cout << motorName << " command sent successfully. Waiting for completion..." << std::endl;
    }
    return true;
}



//位置決め運転のpositionを膨大な数にすることで疑似的な連続運転を実現
bool ModbusCVD::runForDuration(int speed, int direction, int duration_seconds, const std::string& motorName) {
    if (!moveMotor(speed, direction, motorName)) {
        std::cerr << "Failed to control " << motorName << std::endl;
    }
    // 指定時間待機
    std::cout << "Slave " << slaveId << " - Running..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(duration_seconds));
    // モーターを停止
    if (!stopMotor()) {
        std::cerr << "Slave " << slaveId << " - Failed to stop motor after duration." << std::endl;
        return false;
    }
    std::cout << "Slave " << slaveId << " - Motor stopped after " << duration_seconds << " seconds." << std::endl;

    return true;
}

// 静的メソッド：共有コンテキスト作成
modbus_t* ModbusCVD::createSharedContext(const std::string& device, int baudrate) {
    modbus_t* ctx = modbus_new_rtu(device.c_str(), baudrate, 'E', 8, 1);
    if (ctx == nullptr) {
        std::cerr << "Failed to create RTU context: " << modbus_strerror(errno) << std::endl;
        return nullptr;
    }

    // タイムアウト設定（重要：デフォルトは無限待機になる場合がある）
    modbus_set_response_timeout(ctx, 1, 0);  // 1秒
    modbus_set_byte_timeout(ctx, 0, 500000); // 500ms

    if (modbus_connect(ctx) == -1) {
        std::cerr << "Connection failed: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx);
        return nullptr;
    }

    std::cout << "Shared Modbus context created successfully for " << device << std::endl;
    return ctx;
}

// 静的メソッド：共有コンテキスト終了
void ModbusCVD::closeSharedContext(modbus_t* ctx) {
    if (ctx) {
        modbus_close(ctx);
        modbus_free(ctx);
        std::cout << "Shared Modbus context closed." << std::endl;
    }
}

// デバッグ用メソッド
void ModbusCVD::enableDebug(modbus_t* ctx, bool enable) {
    if (ctx) {
        modbus_set_debug(ctx, enable ? TRUE : FALSE);
    }
}