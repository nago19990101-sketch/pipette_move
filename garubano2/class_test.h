#pragma once

using namespace std;

extern double MAG_IMG;

namespace test {
	class MyClass
	{
	public:
		MyClass();
		~MyClass();

		int Xframe_out[3] = { 0,0,0 };	//画像合成スレッドで拡張画像が生成された時：１，されていないとき：0
		Mat Xframe1;
		Mat Xframe2;
		Mat Xframe3;
		bool mouse_wheel_event = FALSE;	//ホイール操作のときTRUE（mouse_callback内）

		//マウスホイール操作を行ったときに存在しない画像へアクセスするのを防止する関数（先行合成処理が有効のときに必要）
		void disable_pre_Xframe_creation(int& pre_Ximage_creation) {
			if (mouse_wheel_event == TRUE) {
				//一定時間（カウント）経過するまで先行合成処理を無効化する
				switch (halt_pre_Xframe_creation_count)
				{
				case 1:
					pre_Ximage_creation = 0;	//先行合成処理を無効化
					halt_pre_Xframe_creation_count--;
					mouse_wheel_event = FALSE;
					//先行合成処理が有効になる
					break;
				default:
					pre_Ximage_creation = 0;	//先行合成処理を無効化
					halt_pre_Xframe_creation_count--;
					break;
				}

			}

		}

		//void init_settings(int t_MAG, int MAG_ExpandedIMG, int  s_MAG_ExpandedIMG, int frame_height, int frame_width) {
		void init_settings(int MAG_ExpandedIMG, int  s_MAG_ExpandedIMG, int frame_height, int frame_width) {
			pre_MAG = MAG_ExpandedIMG / s_MAG_ExpandedIMG;
			frame_height_tmp = frame_height;
			frame_width_tmp = frame_width;
		}


		//拡張画像合成に必要なデータの保存
		void save_ExpCount_data(int store_image_count, int count_Xndata, int expand_viewpoint_num, double MAG) {
			cout << store_image_count << "," << count_Xndata << endl;
			store_image_count_tmp = store_image_count;
			count_Xndata_tmp = count_Xndata;
			expand_viewpoint_num_tmp = expand_viewpoint_num;
			MAG_tmp = MAG;
		};
		
		//拡張画像合成に必要なデータの出力
		void load_ExpCount_data(int &store_image_count, int &count_Xndata, int& expand_viewpoint_num, double& MAG) {
			store_image_count = store_image_count_tmp;
			count_Xndata = count_Xndata_tmp;
			expand_viewpoint_num = expand_viewpoint_num_tmp;
			MAG = MAG_tmp;
		};


		//拡張画像拡大時に画像の範囲を調整するための処理
		void expanded_CV_rect_setting(double MAG, int& center_framex, int& center_framey, int mouse_posx2, int mouse_posy2) {
			if (pre_MAG != MAG) {
				center_framex = center_framex + double(mouse_posx2) * pre_MAG * (1 - (MAG / pre_MAG));
				center_framey = center_framey + double(mouse_posy2) * pre_MAG * (1 - (MAG / pre_MAG));
			}

			if (center_framex + MAG * frame_width_tmp / 2 > frame_width_tmp) {
				center_framex = center_framex - (center_framex + MAG * frame_width_tmp / 2 - frame_width_tmp);
			}
			else if (center_framex - MAG * frame_width_tmp / 2 < 0) {
				center_framex = center_framex - (center_framex - MAG * frame_width_tmp / 2);
			}

			if (center_framey + MAG * frame_height_tmp / 2 > frame_height_tmp) {
				center_framey = center_framey - (center_framey + MAG * frame_height_tmp / 2 - frame_height_tmp);
			}
			else if (center_framey - MAG * frame_height_tmp / 2 < 0) {
				center_framey = center_framey - (center_framey - MAG * frame_height_tmp / 2);
			}


			pre_MAG = MAG;

		};

		//拡張画像上に表示する選択画像の黒枠の設定
		void pointed_rectangle_setting(double MAG, int center_framex, int center_framey, int& center_pointedframex, int& center_pointedframey,
			double& pointed_MAG, int t_pointed_count2, int pointed_count2, double& diff_centerx, double& diff_centery,
			int pointed_pixelx, int pointed_pixely, int npointed_count2) {

			if (t_pointed_count2 != pointed_count2) {
				center_pointedframex = center_framex;
				center_pointedframey = center_framey;
				pointed_MAG = MAG;
			}
			diff_centerx = ((((pointed_pixelx - IMG_WIDTH * MAG_IMG / 2) / IMG_WIDTH / MAG_IMG * frame_width_tmp * pointed_MAG + center_pointedframex) - center_framex) * IMG_WIDTH * MAG_IMG / frame_width_tmp / MAG + IMG_WIDTH * MAG_IMG / 2);
			diff_centery = ((((pointed_pixely - frame_height_tmp * IMG_WIDTH * MAG_IMG / frame_width_tmp / 2) / IMG_WIDTH / MAG_IMG * frame_width_tmp * pointed_MAG + center_pointedframey) - center_framey) * IMG_WIDTH * MAG_IMG / frame_width_tmp / MAG + frame_height_tmp * IMG_WIDTH * MAG_IMG / frame_width_tmp / 2);

		};

	private:
		int store_image_count_tmp = 0;		//確保したメモリ領域において合成時に使用する番号の情報(末尾)
		int count_Xndata_tmp = 0;			//expand_viewpoint_num-1に相当する
		int expand_viewpoint_num_tmp = 0;	//視野拡張枚数
		double MAG_tmp = 1;					//マウスホイール操作によって変化する拡大率の逆数（拡張枚数分の画像が揃ったときの値，親スレッドから子スレッドへの受け渡しに使用）※save_ExpCount_data，load_ExpCount_data，子スレッドの呼び出しタイミング等を変更する場合は，配列に変更することを推奨
		double pre_MAG = 0;					//前回のMAGの値
		int frame_width_tmp = 0;			//合成画像のフレーム幅（元のサイズ）
		int frame_height_tmp = 0;			//合成画像のフレーム高さ（元のサイズ）
		int halt_pre_Xframe_creation_count = 40;	//拡大・縮小処理において先行合成処理を無効化するカウント, 短く設定すると拡大縮小操作時に画像が取得できていない状態で合成処理を行ってしまう．



	};

	MyClass::MyClass()
	{
		
	}

	MyClass::~MyClass()
	{
	}
}

