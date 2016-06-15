﻿/**************************************--Žd—l--***************************************
	プロジェクトの種類：Win32コンソールアプリケーション
Debug時ライブラリ：libARd.lib libARgsubd.lib libARvideod.lib opengl32.lib glu32.lib glut32.lib
Release時ライブラリ：libAR.lib libARgsub.lib libARvideo.lib opengl32.lib glu32.lib glut32.lib 
*************************************************************************************/

#pragma warning(disable:4819)

#include "knVideo.h"
#ifdef _WIN32 
#include <windows.h> // standard system header file
#endif 
#define _USE_MATH_DEFINES	// math.hのM_PIを使うため
#include <math.h>			// 角度計算用

#include <stdio.h>
#include <stdlib.h>
#include <iostream> 
#include <AR/arMulti.h>
#include <AR/matrix.h>
#include <GL/glu.h>
#ifndef __APPLE__ 
#include <GL/gl.h> 
#include <GL/glut.h> 
#else 
#include <OpenGL/gl.h> 
#include <GLUT/glut.h> 
#endif 
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/param.h>
//#include "stdafx.h"

using namespace std;

// グローバル変数
/* カメラ構成 */
char *vconf_name  = "Data\\WDM_camera_flipV.xml";	// ビデオデバイスの設定ファイル
int  xsize;											// ウィンドウサイズ
int  ysize;											// ウィンドウサイズ
int  thresh = 100;									// 2値化の閾値, can be lower of higher
int  counta = 0;										// 処理フレーム数

/* カメラパラメータ */
char *cparam_name = "Data/camera_para.dat";
// カメラパラメータファイル
ARParam cparam;										// カメラパラメータ

/* パターンファイル (Multi Marker)*/
#define MARK_NUM		3						// 使用するマーカーの個数
													//-----
#define MARK1_MARK_ID	1						// マーカーID
#define MARK1_PATT_NAME	"Data\\patt.r"		// パターンファイル名
#define MARK1_SIZE		80.0					// パターンの幅（80mm）
													//-----
#define MARK2_MARK_ID	2						// マーカーID
#define MARK2_PATT_NAME	"Data\\patt.t3"	// パターンファイル名
#define MARK2_SIZE		80.0					// パターンの幅（80mm）
													//-----
#define MARK3_MARK_ID	3						// マーカーID
#define MARK3_PATT_NAME	"Data\\patt.f1"		// パターンファイル名
#define MARK3_SIZE		80.0					// パターンのサイズ（単位：ｍｍ）
													//-----
typedef struct {
	char   *patt_name;			// パターンファイル, empty, will be define later
	int    patt_id;				// パターンのID
	int    mark_id;				// マーカーID
	int    visible;				// 検出フラグ
	double patt_width;			// パターンのサイズ（単位：ｍｍ）
	double patt_center[2];		// パターンの中心座標, empty, will be define later
	double patt_trans[3][4];	// 座標変換行列
} MARK_T;
//-----
MARK_T   marker[MARK_NUM] = {
	{ MARK1_PATT_NAME, -1, MARK1_MARK_ID, 0, MARK1_SIZE,{ 0.0, 0.0 } },
	{ MARK2_PATT_NAME, -1, MARK2_MARK_ID, 0, MARK2_SIZE,{ 0.0, 0.0 } },
	{ MARK3_PATT_NAME, -1, MARK3_MARK_ID, 0, MARK3_SIZE,{ 0.0, 0.0 } }
}; 

/* パターンファイル(single marker)*/
//char   *patt_name = "Data/patt.hiro";             // パターンファイル
//int    patt_id;                                     // パターンのID
//double patt_trans[3][4];                            // 座標変換行列
//double patt_center[2] = { 0.0, 0.0 };               // パターンの中心座標
//double patt_width = 80.0;                       // パターンのサイズ（単位：ｍｍ）


// some other functions
void Init(void);
void MainLoop(void);
void SetupLighting(void);
void SetupMaterial(void);
void SetupLighting1(void);
void SetupLighting2(void);
void SetupMaterial1(void);
void SetupMaterial2(void);
void KeyEvent(unsigned char key, int x, int y);
void MouseEvent(int button, int state, int x, int y);
void Cleanup(void);
void DrawObject(void);
void DrawObject(int mark_id, double patt_trans[3][4]);


//=======================================================
// main関数
//=======================================================
int main( int argc, char **argv )
{
	// GLUTの初期化
	glutInit( &argc, argv );

	// ARアプリケーションの初期化
	Init();

	// ビデオキャプチャの開始
	knVideoCapStart();

	// メインループの開始
	argMainLoop( MouseEvent, KeyEvent, MainLoop );

	return 0;
}


//=======================================================
// 初期化関数
//=======================================================
void Init(void)
{
	ARParam wparam;		// カメラパラメータ

	// ビデオデバイスの設定
	if( knVideoOpen( vconf_name ) < 0 ){
		printf("ビデオデバイスのエラー\n");
		exit(0);
	}

	// ウィンドウサイズの取得
	if( knVideoInqSize( &xsize, &ysize ) < 0 ) exit(0);
	printf("Image size (x,y) = (%d,$d)\n", xsize, ysize);

	// カメラパラメータの設定
	if( arParamLoad( cparam_name, 1, &wparam ) < 0 ){
		printf("カメラパラメータの読み込みに失敗しました\n");
		exit(0);
	}

	// カメラパラメータのサイズ調整
	arParamChangeSize( &wparam, xsize, ysize, &cparam );
	// カメラパラメータの初期化
	arInitCparam( &cparam );
	printf("*** Camera Parameter ***\n");
	arParamDisp( &cparam );

	// パターンファイルのロード(single marker)
	/*if( (patt_id = arLoadPatt(patt_name)) < 0){
		printf("パターンファイルの読み込みに失敗しました\n");
		exit(0);
	}*/

	// パターンファイルのロード(multi markers)
	for (int i = 0; i<MARK_NUM; i++) {
		if ((marker[i].patt_id = arLoadPatt(marker[i].patt_name)) < 0) {
			printf("パターンファイルの読み込みに失敗しました\n");
			printf("%s\n", marker[i].patt_name);
			exit(0);
		}
	}

	// gsubライブラリの初期化, change 0 when want to have multiple display area
	argInit( &cparam, 1.0, 0, 0, 0, 0 );

	// ウィンドウタイトルの設定
	glutSetWindowTitle("ARTK_basic");
}


//=======================================================
// メインループ関数
//=======================================================
void MainLoop(void)
{
	ARUint8          *image;			// カメラキャプチャ画像
	ARMarkerInfo     *marker_info;		// マーカ情報
	int              marker_num;		// 検出されたマーカの数
	int              i, j, k;
	//static bool		 isFirstDetect = true; // to determine if first detect(single marker)

	// カメラ画像の取得
	if( (image = (ARUint8 *)knVideoGetImage()) == NULL ){
		arUtilSleep( 2 );
		return;
	}
	if( counta == 0 ) arUtilTimerReset();
	counta++;

	// カメラ画像の描画
	argDrawMode2D();
	argDispImage( image, 0, 0 );

	// マーカの検出と認識
	if( arDetectMarker( image, thresh, &marker_info, &marker_num ) < 0 ){
		Cleanup();
		exit(0);
	}

	// 次の画像のキャプチャ指示
	knVideoCapNext();

	// 3Dオブジェクトを描画するための準備(Multi markers)
	argDrawMode3D();
	argDraw3dCamera(0, 0);
	glClearDepth(1.0);					// デプスバッファの消去値
	glClear(GL_DEPTH_BUFFER_BIT);		// デプスバッファの初期化

	// マーカの一致度の比較(single marker)
	/*k = -1;
	for( j = 0; j < marker_num; j++ ){
		if( patt_id == marker_info[j].id ){
			if( k == -1 ) k = j;
			else if( marker_info[k].cf < marker_info[j].cf ) k = j;
		}
	}*/

	// マーカの一致度の比較(Multi markers)
	for (i = 0; i < MARK_NUM; i++) {
		k = -1;
		for (j = 0; j < marker_num; j++) {
			if (marker[i].patt_id == marker_info[j].id) {
				if (k == -1) k = j;
				else if (marker_info[k].cf < marker_info[j].cf) k = j;
			}
		}

		// マーカーが見つからなかったとき(single marker)
		/*if( k == -1 ){
			argSwapBuffers();
			return;
		}*/

		// マーカーが見つからなかったとき(Multi markers)
		if (k == -1) {
			marker[i].visible = 0;
			continue;
		}

		// マーカの位置・姿勢（座標変換行列）の計算(single marker)
		//arGetTransMat( &marker_info[k], patt_center, patt_width, patt_trans );

		// マーカの位置・姿勢（座標変換行列）の計算(single marker)
		/*if (isFirstDetect) {
			arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);
		}
		else {
			arGetTransMatCont(&marker_info[k], patt_trans, patt_center, patt_width, patt_trans);
		}*/
		// 1st patt_trans = prev_conv：前回求めたマーカ→ビュー座標変換行列
		// 2nd patt_trans = conv：新しいマーカ→ビュー座標変換行列
		//isFirstDetect = false; // 初回フラグ無効化

		// 座標変換行列を取得(Multi markers)
		if (marker[i].visible == 0) {
			// 1フレームを使ってマーカの位置・姿勢（座標変換行列）の計算
			arGetTransMat(&marker_info[k], marker[i].patt_center, marker[i].patt_width, marker[i].patt_trans);
		}
		else {
			// 前のフレームを使ってマーカの位置・姿勢（座標変換行列）の計算
			arGetTransMatCont(&marker_info[k], marker[i].patt_trans, marker[i].patt_center, marker[i].patt_width, marker[i].patt_trans);
		}
		marker[i].visible = 1; // use int 0 and 1 instead of boolean
		
		// 3Dオブジェクトの描画(Multi markers)
		DrawObject(marker[i].mark_id, marker[i].patt_trans);

	// 3Dオブジェクトの描画(single marker)
		//DrawObject();
	}

	// バッファの内容を画面に表示
	argSwapBuffers();

	/*　↓　Multi Markers の場合使用*/
	// 2つのマーカ間の距離を表示（マーカ1　とマーカ2　を認識した場合）
	if (marker[0].visible > 0 && marker[1].visible > 0) {
		double wmat1[3][4], wmat2[3][4];

		// ビュー→マーカ行列（カメラ座標系を基準に考えたマーカの位置・姿勢）を取得
		arUtilMatInv(marker[0].patt_trans, wmat1);
		// マーカ1座標系を基準に考えたマーカ2の位置・姿勢（＝マーカ1とマーカ2の距離・角度の差）を取得
		arUtilMatMul(wmat1, marker[1].patt_trans, wmat2);

		// 距離を表示(x, y, z)
		printf("%5.4lf[mm] %5.4lf[mm] %5.4lf[mm]\n", wmat2[0][3], wmat2[1][3], wmat2[2][3]);
	}

	// 2つのマーカ間の角度の差を表示（マーカ1　とマーカ3　を認識した場合）
	if (marker[0].visible > 0 && marker[2].visible > 0) {
		double wmat1[3][4], wmat2[3][4];
		double yaw, pitch, roll;

		// ビュー→マーカ行列（カメラ座標系を基準に考えたマーカの位置・姿勢）を取得
		arUtilMatInv(marker[0].patt_trans, wmat1);
		// マーカ1座標系を基準に考えたマーカ3の位置（＝マーカ1とマーカ3の距離・姿勢の差）を取得
		arUtilMatMul(wmat1, marker[2].patt_trans, wmat2);

		// 姿勢の差を表示(show 3x3 transformation matrix)
		for( i=0; i<3; i++ ) {
		    for( j=0; j< 3; j++ ) printf("%5.4f ", wmat2[i][j]);
		    printf("\n");
		}
		printf("\n");

		// 角度の差を表示（-180°～180°）
		yaw = atan2(wmat2[1][0], wmat2[0][0]);
		pitch = atan2(wmat2[2][1], wmat2[2][2]);
		roll = atan2(wmat2[2][0], sqrt(wmat2[2][1] * wmat2[2][1] + wmat2[2][2] * wmat2[2][2]));

		printf("yaw = %4.4lf pitch = %4.4lf roll = %4.4lf\n", 180.0*yaw / M_PI, 180.0*pitch / M_PI, 180.0*roll / M_PI);
	}
	/*　↑　Multi Markers の場合使用*/
}

//=======================================================
// 3Dオブジェクトの描画を行う関数(Multi markers)
//=======================================================
void DrawObject(int mark_id, double patt_trans[3][4])
{
	double gl_para[16];	// ARToolKit->OpenGL変換行列

	// 陰面消去
	glEnable(GL_DEPTH_TEST);			// 陰面消去・有効
	glDepthFunc(GL_LEQUAL);				// デプステスト

	// 変換行列の適用
	argConvGlpara(patt_trans, gl_para);	// ARToolKitからOpenGLの行列に変換
	glMatrixMode(GL_MODELVIEW);			// 行列変換モード・モデルビュー
	glLoadMatrixd(gl_para);				// 読み込む行列を指定

	switch (mark_id) {
	case MARK1_MARK_ID:
		// ライティング
		SetupLighting1();			// ライトの定義
		glEnable(GL_LIGHTING);		// ライティング・有効
		glEnable(GL_LIGHT0);		// ライト0・オン
									// オブジェクトの材質
		SetupMaterial1();

		// 3Dオブジェクトの描画
		glTranslatef(0.0, 0.0, 25.0);	// マーカの上に載せるためにZ方向（マーカ上方）に25.0[mm]移動
		glutSolidCube(50.0);			// ソリッドキューブを描画（1辺のサイズ50[mm]）
		break;

	case MARK2_MARK_ID:
		// ライティング
		SetupLighting2();			// ライトの定義
		glEnable(GL_LIGHTING);		// ライティング・有効
		glEnable(GL_LIGHT0);		// ライト0・オン
									// オブジェクトの材質
		SetupMaterial2();

		// 3Dオブジェクトの描画
		glTranslatef(0.0, 0.0, 25.0);		// マーカの上に載せるためにZ方向（マーカ上方）に25.0[mm]移動
		glutSolidSphere(50.0, 10, 10);		// ソリッドスフィアを描画（1辺のサイズ50[mm]）
		break;

	case MARK3_MARK_ID:
		// ライティング
		SetupLighting1();			// ライトの定義
		glEnable(GL_LIGHTING);		// ライティング・有効
		glEnable(GL_LIGHT0);		// ライト0・オン
									// オブジェクトの材質
		SetupMaterial2();

		// 3Dオブジェクトの描画
		glTranslatef(0.0, 0.0, 25.0);	// マーカの上に載せるためにZ方向（マーカ上方）に25.0[mm]移動
		glRotated(90, 1.0, 0.0, 0.0);	// ティーポットをマーカ上に載せるために90°回転
		glutSolidTeapot(50.0);			// ソリッドティーポットを描画（サイズ50[mm]）
		break;
	}


	// 終了処理
	glDisable(GL_LIGHTING);			// ライティング・無効
	glDisable(GL_DEPTH_TEST);		// デプステスト・無効
}

//=======================================================
// 3Dオブジェクトの描画を行う関数(single marker)
//=======================================================
//void DrawObject(void)
//{
//	double gl_para[16];	// ARToolKit->OpenGL変換行列
//
//	// 3Dオブジェクトを描画するための準備
//	argDrawMode3D();
//	argDraw3dCamera( 0, 0 );
//
//	// 陰面消去
//	glClearDepth(1.0);					// デプスバッファの消去値
//	glClear( GL_DEPTH_BUFFER_BIT );		// デプスバッファの初期化
//	glEnable (GL_DEPTH_TEST );			// 陰面消去・有効
//	glDepthFunc( GL_LEQUAL );			// デプステスト
//
//	// •ÏŠ·s—ñ‚Ì“K—p
//	argConvGlpara( patt_trans, gl_para );	// ARToolKitからOpenGLの行列に変換
//	glMatrixMode( GL_MODELVIEW );			// 行列変換モード・モデルビュー
//	glLoadMatrixd( gl_para );				// 読み込む行列を指定
//
//	// マーカ→ビュー座標変換行列を表示
//	static int h_count;
//	if( h_count > 10 ){		// 大量に出てくるの防止用
//		for ( int i = 0; i < 4; i++ ){
//			printf(" ( ");
//			for( int j = 0; j < 4; j++ ){
//				printf("%10.4lf", gl_para[(j*4)+i]);
//			}
//			printf(" ) \n");
//		}
//		h_count = 0;
//		printf("\n");
//	}
//	h_count++;
//
//	// ライティング
//	SetupLighting();			// ライトの定義
//	glEnable( GL_LIGHTING );	// ライティング・有効
//	glEnable( GL_LIGHT0 );		// ライト0・オン
//
//	// オブジェクトの材質
//	SetupMaterial();
//
//	// 3Dオブジェクトの描画
//	glTranslatef( 0.0, 0.0, 20.0 );	// マーカの上に載せるためにZ方向（マーカ上方）に20.0[mm]移動
//	glutSolidCube( 50.0 );			// ソリッドキューブを描画（1辺のサイズ50[mm]）
//
//	// 終了処理
//	glDisable( GL_LIGHTING );		// ライティング・無効
//	glDisable( GL_DEPTH_TEST );		// デプステスト・無効
//}

//=======================================================
// ライティング(Multi markers)
//=======================================================
void SetupLighting1(void)
{
	// ライトの定義
	GLfloat lt0_position[] = { 100.0, -200.0, 200.0, 0.0 };	// ライト0の位置
	GLfloat lt0_ambient[] = { 0.1, 0.1, 0.1, 1.0 };			// 　　　　 環境光
	GLfloat lt0_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };			// 　　　　 拡散光

															// ライトの設定
	glLightfv(GL_LIGHT0, GL_POSITION, lt0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lt0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lt0_diffuse);
}

void SetupLighting2(void)
{
	// ライトの定義
	GLfloat lt0_position[] = { 100.0, 200.0, 200.0, 0.0 };	// ライト0の位置
	GLfloat lt0_ambient[] = { 0.2, 0.2, 0.2, 1.0 };			// 　　　　 環境光
	GLfloat lt0_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };			// 　　　　 拡散光

	// ライトの設定
	glLightfv(GL_LIGHT0, GL_POSITION, lt0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lt0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lt0_diffuse);
}


//=======================================================
// マテリアルの設定(Multi markers)
//=======================================================
void SetupMaterial1(void)
{
	// オブジェクトの材質
	GLfloat mat_ambient[] = { 0.0, 1.0, 1.0, 1.0 };	// 材質の環境光
	GLfloat mat_specular[] = { 0.0, 0.0, 1.0, 1.0 };	// 鏡面光
	GLfloat mat_shininess[] = { 50.0 };				// 鏡面係数

	// マテリアルの設定
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void SetupMaterial2(void)
{
	// オブジェクトの材質
	GLfloat mat_ambient[] = { 0.0, 0.0, 1.0, 1.0 };	// 材質の環境光
	GLfloat mat_specular[] = { 0.0, 0.0, 1.0, 1.0 };	// 鏡面光
	GLfloat mat_shininess[] = { 50.0 };				// 鏡面係数

	// マテリアルの設定
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}


//=======================================================
// ライティング(single marker)
//=======================================================
//void SetupLighting(void)
//{
//	// 
//	GLfloat lt0_position[] = {100.0, -200.0, 200.0, 0.0};	// ライト0の位置
//	GLfloat lt0_ambient[]  = {0.1, 0.1, 0.1, 1.0};			// 環境光
//	GLfloat lt0_diffuse[]  = {0.8, 0.8, 0.8, 1.0};			// 拡散光
//
//	// ƒ‰ƒCƒg‚ÌÝ’è
//	glLightfv( GL_LIGHT0, GL_POSITION, lt0_position );
//	glLightfv( GL_LIGHT0, GL_AMBIENT, lt0_ambient );
//	glLightfv( GL_LIGHT0, GL_DIFFUSE, lt0_diffuse );
//}


//=======================================================
// マテリアルの設定(single marker)
//=======================================================
//void SetupMaterial(void)
//{
//	// オブジェクトの材質
//	GLfloat mat_ambient[] = {0.0, 0.0, 1.0, 1.0};	// 材質の環境光
//	GLfloat mat_specular[] = {0.0, 0.0, 1.0, 1.0};	// 鏡面光
//	GLfloat mat_shininess[] = {50.0};				// 鏡面係数
//
//	// マテリアルの設定
//	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
//}


//=======================================================
// キーボード入力処理関数
//=======================================================
void KeyEvent( unsigned char key, int x, int y )
{
	// ESCキーを入力したらアプリケーション終了
	if (key == 0x1b ){
		printf("*** %f (frame/sec)\n", (double)counta/arUtilTimer());
		Cleanup();
		exit(0);
	}
}


//=======================================================
// マウス入力処理関数
//=======================================================
void MouseEvent( int button, int state, int x, int y )
{
	// 入力状態を表示
	printf("ボタン：%d 状態：%d 座標：(x,y)=(%d,%d) \n", button, state, x, y );
}


//=======================================================
// 終了処理関数
//=======================================================
void Cleanup(void)
{
	knVideoCapStop();	// ビデオキャプチャの停止
	knVideoClose();		// ビデオデバイスの終了
	argCleanup();		// ARToolKitの終了処理
}