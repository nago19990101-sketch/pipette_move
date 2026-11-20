#ifndef MODBUSCVD_H
#define MODBUSCVD_H

#include "modbus.h"
#include <string>
#include <mutex>

class ModbusCVD {
public:
    // 既存のコンストラクタ（後方互換性のため残す）
    ModbusCVD(const std::string& device, int baudrate, int slaveId);

    // 新しいコンストラクタ：共有コンテキスト用
    ModbusCVD(modbus_t* sharedCtx, int slaveId);

    ~ModbusCVD();

    bool resetAlarm();
    bool free(bool sw);
    bool writeParamAcc(int time_ms, int speed_hz = 16667);
    bool writeParamDec(int time_ms, int speed_hz = 16667);
    bool writeParamCurrent(int current);
    bool startPosition(int position, int speed, int OpeType = 1);
    bool stopMotor();
    bool moveMotor(int speed, int direction, const std::string& motorName);

    bool writeRegs(int addr, const uint16_t* data, int len);
    bool writeRegs32(int addr, const int32_t* data, int len);
    bool initializeAndStart(int position, int speed, int mode, const std::string& motorName);
    bool runForDuration(int speed, int direction, int duration_seconds, const std::string& motorName);

    // 静的メソッド：共有コンテキスト作成用
    static modbus_t* createSharedContext(const std::string& device, int baudrate);
    static void closeSharedContext(modbus_t* ctx);

    // デバッグ用メソッド
    static void enableDebug(modbus_t* ctx, bool enable = true);

    // 保持レジスタを読み込むための関数
    bool readHoldingRegisters(int address, uint16_t* value, int num_registers);

private:
    modbus_t* ctx;
    int slaveId;
    uint16_t driveCmd;
    int rateAcc;
    int rateDec;
    int OpeCurrent;
    bool ownsContext;  // このインスタンスがコンテキストを所有するかどうか

    // スレッドセーフのためのミューテックス（静的：全インスタンスで共有）
    static std::mutex modbusMutex;

    // 共通の初期化処理
    void initialize();
};

#endif // MODBUSCVD_H