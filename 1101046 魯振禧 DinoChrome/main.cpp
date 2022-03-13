/* Press space key or up key of down key to start the game. 
   Space key and up key is to jump.
   Down key is to duck.
   The player (which is you) is going to be controlling a Tyrannosaurus rex.
   The aim of the game is to avoid all obstacles in the game which include cactus and pterosaurs.
   If you hit into any of the obstacles, it is GAME OVER.
   
   Zephyn Lo    Start: 14/05/2019  Submit: 05/06/2019 */




#include<windows.h>
#include<stdio.h>
#include<d3dx9.h>
#include<time.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#define	SCRW		1280	// ウィンドウ幅（Width
#define	SCRH		720		// ウィンドウ高さ（Height

LPDIRECT3D9				lpD3D;		// Direct3Dインターフェイス

LPDIRECT3DDEVICE9		lpD3DDevice;	// Direct3DDeviceインターフェイス

D3DPRESENT_PARAMETERS d3dpp;


// 頂点フォーマットの定義
struct VERTEX
{
	D3DXVECTOR3 Pos;
	D3DCOLOR Color;
	D3DXVECTOR2 Tex;
};


////  グローバル変数宣言

LPD3DXSPRITE lpSprite;	// スプライト
LPD3DXFONT lpFont;		// フォント

int gameStart;

LPDIRECT3DTEXTURE9 playerTex;
LPDIRECT3DTEXTURE9 playerNightTex;

D3DXMATRIX playerMat;	//playerMat is 行列
D3DXMATRIX playerStartMat;
D3DXMATRIX playerGroundMat;
D3DXMATRIX playerMoveMat;
D3DXMATRIX playerDuckMat;

int playerStartAnime;
int playerStartAnimeWait;

float playerX;
float playerY;

float playerMoveY;

int playerFlg;

int playerAnime;
int playerAnimeWait;

int jumpFlg;

int duckFlg;
int playerDuckAnime;
int playerDuckAnimeWait;

int gameScore;
int highScore;

D3DXMATRIX groundMat;
float groundX;
float groundY;

float groundSpeed;
float animeSpeed;
float enemyWaitSpeed;

const int cloudNum = 4;
D3DXMATRIX cloudMat[cloudNum];
float cloudX[cloudNum];
float cloudY[cloudNum];
int cloudFlg[cloudNum];

const int enemyNum = 4;
D3DXMATRIX enemy1Mat;
D3DXMATRIX enemy2Mat;
D3DXMATRIX enemy3Mat;
D3DXMATRIX enemy4Mat;
D3DXMATRIX enemyFlyMat[enemyNum];

float enemyX[enemyNum];
float enemyY;
int enemyFlg[enemyNum];
int enemyWait;
int enemyType;

int rnd;

int nightFlg;
int nightTimes;

float enemyFlyX[enemyNum];
float enemyFlyY[enemyNum];
int enemyFlyFlg[enemyNum];
int enemyFlyAnime[enemyNum];
int enemyFlyAnimeWait[enemyNum];
int enemyFlyWait;

D3DXMATRIX gameOverMat;
D3DXMATRIX replayMat;

FILE *fp;

bool gameFullScreen;	// フルスクリーン（true,false)





#define	FVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)


//Function to reset the game
void GameReset(void)
{
	//groundY default
	groundY = 650;

	//cloudX and cloudY default
	for (int c = 0; c < cloudNum; c++)
	{
		cloudY[c] = 200;
		cloudX[c] = 1280;
	}

	//flag for player
	playerFlg = 1;

	gameStart = 0;

	//setting the default position of the player
	playerX = 1280 / 6 - 80 / 2;  //window length size/2 - sprite length size/2
	playerY = 720 / 1.1538 - 93 / 2;  //window height size/2 - sprite height size/2

	//start with left sprite
	playerAnime = 1;

	playerStartAnime = 1;

	playerDuckAnime = 1;

	//default position of enemy
	for (int e = 0; e < enemyNum; e++)
	{
		enemyX[e] = 1280;  //window length size/2 - sprite length size/2
	}
	enemyY = 660 - 95;  //window height size/2 - sprite height size/2

	//set all enemy to not rendered at the begining
	for (int e = 0; e < enemyNum; e++)
	{
		enemyFlg[e] = 0;
	}

	//set the default X and Y and flag for flying dinosaur
	for (int ef = 0; ef < enemyNum; ef++)
	{
		enemyFlyX[ef] = 1280;
		enemyFlyY[ef] = 720 / 1.1638 - 93 / 2;
		enemyFlyFlg[ef] = 0;
	}

	//set the probability of enemy appearing to 0
	rnd = 0;

	//the score will be 0 when the game starts
	gameScore = 0;

	//the speed of ground movement
	groundSpeed = 8;

	//speed of animeation
	animeSpeed = 8;

	//to set highscore as the highscore saved
	fp = fopen("HighScore.txt", "r");
	if (fp == NULL)
	{
		printf("Cannot open HighScore file. \n");
	}
	fscanf(fp, "%d", &highScore);
	fclose(fp);

	//so that enemy won't appear that often
	enemyWait = 0;
	enemyFlyWait = 0;
	enemyWaitSpeed = 100;

	//set default as 0
	jumpFlg = 0;

	//to see how long button has been pressed
	duckFlg = 0;

	//how many times has it turned to night
	nightTimes = 0;
}

// 更新処理
void Update(void)
{
	if (gameStart == 0)
	{
		if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			GameReset();
			gameStart = 1;
		}

		//animation for player blinking on the menu screen
		playerStartAnimeWait--;
		if (playerStartAnimeWait < 0)
		{
			playerStartAnimeWait = 0;

			if (playerStartAnime == 0)
			{
				playerStartAnime = 1;
				playerStartAnimeWait = 10;
			}
			else if (playerStartAnime == 1)
			{
				playerStartAnime = 0;
				playerStartAnimeWait = 120;
			}

		}

	}
	else
	{
		if (playerFlg == 1)
		{

	/*		this is for if you want the player can either do a jump or long jump
			e.g. either -15 or -20, but there isn't -16, -17, -18, -19

			if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			{

				Flg = 1;
				Cnt++;

				if (Cnt > 60)
				{
					jumpPower = -30:

				}
			}
			else {
				if (Flg == 1)
				{
					Flg = 0;

					if (Cnt > 30)
					{
						jumpPower= -30:
					}
					else {
						jumpPower = -10:

					}
				}

			}
*/
			//player jump movement
			if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(VK_SPACE) & 0x8000)
			{
				if (jumpFlg <= 3)
				{
						jumpFlg++;
						playerMoveY = -15;
				}
			}
			//so that the player drops back down
			playerMoveY++;

			//moving the y position of the player
			playerY += playerMoveY;

			if (playerY >= 720 / 1.1538 - 93 / 2)
			{
				playerY = 720 / 1.1538 - 93 / 2;
				playerMoveY = 0;
				jumpFlg = 0;
			}

			//so that the player can't get out of screen
			if (playerX < 0)
			{
				playerX = 0;
			}
			else if (playerX > 1280 / 6 - 80 / 2)
			{
				playerX = 1280 / 6 - 80 / 2;
			}
			
			if (groundX <= 0)
			{
				groundX += 2404;
			}
			else if (groundX >= 1280)
			{
				groundX += 0;  
			}



			gameScore++;
			groundX -= groundSpeed;

			//what happens every +100 score
			if (gameScore % 100 == 0)
			{
				//groundSpeed += 1;
				groundSpeed *= 1.1f;
				animeSpeed /= 1.1f;
				enemyWaitSpeed /= 1.1f;
			}

			//player duck
			if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			{
				duckFlg = 1;

				if (jumpFlg >= 1)
				{
					playerMoveY += 5;
				}

				//animation for player ducking
				playerDuckAnimeWait--;
				if (playerDuckAnimeWait < 0)
				{
					playerDuckAnimeWait = 0;

					if (playerDuckAnime == 0)
					{
						playerDuckAnime = 1;
						playerDuckAnimeWait = animeSpeed;
					}
					else if (playerDuckAnime == 1)
					{
						playerDuckAnime = 0;
						playerDuckAnimeWait = animeSpeed;
					}

				}
			}
			else
			{
				duckFlg = 0;
			}
				
			


			//appearence of enemy
			if (enemyWait == 0)
			{
				rnd = rand() % 4;

				//switch to show which enemy
				switch (rnd)
				{
				case 0:
					if (enemyFlg[0] == 0)
					{
						enemyFlg[0] = 1;
						enemyWait = enemyWaitSpeed;
					}
					break;
				case 1:
					if (enemyFlg[1] == 0)
					{
						enemyFlg[1] = 1;
						enemyWait = enemyWaitSpeed;
					}
					break;
				case 2:
					if (enemyFlg[2] == 0)
					{
						enemyFlg[2] = 1;
						enemyWait = enemyWaitSpeed;
					}
					break;
				case 3:
					if (enemyFlg[3] == 0)
					{
						enemyFlg[3] = 1;
						enemyWait = enemyWaitSpeed;
					}
					break;
				}
			}

			for (int e = 0; e < enemyNum; e++)
			{
				//speed of movement and back to default position after out of screen
				if (enemyFlg[e] == 1)
				{
					enemyX[e] -= groundSpeed;
					if (enemyX[e] < -150)
					{
						enemyFlg[e] = 0;
						enemyX[e] = 1280;
					}
				}
			}

			//how long until the next enemy appears
			enemyWait--;
			if (enemyWait < 0)
			{
				enemyWait = enemyWaitSpeed;
			}

			//check has enemy hit the player
			/*for (int e = 0; e < enemyNum; e++)
			{
				if (enemyFlg[e] == 1)
				{
					if ((enemyX[0] + 47 > playerX) && (enemyX[0] < playerX + 80) && (enemyY + 94 > playerY) && (enemyY < playerY))
					{
						playerFlg = 0;
					}
					if ((enemyX[1] + 95 > playerX) && (enemyX[1] < playerX + 80) && (enemyY + 94 > playerY) && (enemyY < playerY))
					{
						playerFlg = 0;
					}
					if ((enemyX[2] + 99 > playerX) && (enemyX[2] < playerX + 80) && (enemyY + 95 > playerY) && (enemyY < playerY))
					{
						playerFlg = 0;
					}
					if ((enemyX[3] + 147 > playerX) && (enemyX[3] < playerX + 80) && (enemyY + 95 > playerY) && (enemyY < playerY))
					{
						playerFlg = 0;
					}
				}
			}*/

			//appearance of flying dinosaur
			if (gameScore >= 500)
			{
				if (enemyFlyWait == 0)
				{
					if (rand() % 100 < 1)
					{
						for (int ef = 0; ef < enemyNum; ef++)
						{
							if (enemyX[ef] <= 1080 && enemyFlyFlg[ef] == 0)
							{
								enemyFlyFlg[ef] = 1;
								enemyFlyY[ef] = rand() % (550 - 400) + 400;
								enemyFlyWait = 100;
								break;
							}
						}
					}
				}
			}

			enemyFlyWait--;
			if (enemyFlyWait < 0)
			{
				enemyFlyWait = 0;
			}

			//movement of flying dinosaur
			for (int ef = 0; ef < enemyNum; ef++)
			{
				if (enemyFlyFlg[ef] == 1)
				{
					enemyFlyX[ef] -= groundSpeed;

					//animation for flying dinosaur
					enemyFlyAnimeWait[ef]--;
					if (enemyFlyAnimeWait[ef] < 0)
					{
						enemyFlyAnimeWait[ef] = 0;

						if (enemyFlyAnime[ef] == 0)
						{
							enemyFlyAnime[ef] = 1;
							enemyFlyAnimeWait[ef] = animeSpeed;
						}
						else if (enemyFlyAnime[ef] == 1)
						{
							enemyFlyAnime[ef] = 0;
							enemyFlyAnimeWait[ef] = animeSpeed;
						}
					}

					if (enemyFlyX[ef] < 0 - 93)
					{
						enemyFlyFlg[ef] = 0;
						enemyFlyX[ef] = 1280;
					}
				}
			}

			//check has flying dinosaur hit the player
			for (int ef = 0; ef < enemyNum; ef++)
			{
				if (enemyFlyFlg[ef] == 1)
				{
					if ((enemyFlyX[ef] + 93 > playerX) && (enemyFlyX[ef] < playerX + 80) && (enemyFlyY[ef] + 40 > playerY) && (enemyFlyY[ef] < playerY))
					{
						playerFlg = 0;
					}
				}

			}

			//animation for player movement
			playerAnimeWait--;
			if (playerAnimeWait < 0)
			{
				playerAnimeWait = 0;

				if (playerAnime == 0)
				{
					playerAnime = 1;
					playerAnimeWait = animeSpeed;
				}
				else if (playerAnime == 1)
				{
					playerAnime = 0;
					playerAnimeWait = animeSpeed;
				}

			}

			//random appearance of cloud
			if (rand() % 100 < 2)
			{
				for (int c = 0; c < cloudNum; c++)
				{
					if (cloudFlg[c] == 0)
					{
						cloudFlg[c] = 1;
						cloudX[c] = 1280;
						cloudY[c] = rand() % 360;
						break;
					}
				}
			}

			//movement of cloud
			for (int c = 0; c < cloudNum; c++)
			{
				if (cloudFlg[c] == 1)
				{
					cloudX[c] -= groundSpeed;
					if (cloudX[c] < 0 - 92)
					{
						cloudFlg[c] = 0;
					}
				}
			}

			if (nightFlg == 0)
			{
				if (gameScore >= 1000 + nightTimes * 2000)
				{
					nightFlg = 1;
					nightTimes++;
				}
			}
			else
			{
				if (gameScore >= nightTimes * 2000 - 500)
				{
					nightFlg = 0;
				}
			}

		}
		else if (playerFlg == 0)
		{
			
			if (gameScore > highScore)
			{
				highScore = gameScore;

				fp = fopen("HighScore.txt", "w");
				if (fp == NULL)
				{
					printf("Cannot open HighScore file. \n");
				}
				fprintf(fp, "%d", highScore);

				fclose(fp);
			}

			

			if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(VK_SPACE) & 0x8000)
			{
				GameReset();
			}
		}
	}
	
}

// 3D描画
void Render3D(void)
{

}

// 2D描画
void Render2D(void)
{
	//////////////////////////////////////////////////
	///	スプライトの描画処理
	//////////////////////////////////////////////////


	if (gameStart == 0)
	{
		// 描画開始
		lpSprite->Begin(D3DXSPRITE_ALPHABLEND);

		RECT rcDinoGround = { 1298, 112, 1387, 129 };
		D3DXMatrixTranslation(&playerGroundMat, 1280 / 6, (720 / 3) + 80, 0);
		lpSprite->SetTransform(&playerGroundMat);
		lpSprite->Draw(playerTex, &rcDinoGround, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

		//render second background and loop it to make a moving background
		RECT rcDinoStart = { 1338 + (playerStartAnime * 88), 2, 1426 + (playerStartAnime * 88), 96 };
		D3DXMatrixTranslation(&playerStartMat, 1280 / 6, 720 / 3, 0);
		lpSprite->SetTransform(&playerStartMat);
		lpSprite->Draw(playerTex, &rcDinoStart, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 描画終了
		lpSprite->End();

		//文字表示はここから

		RECT rcStart = { 1280/9, 720/2, 100, 100 };
		char startText[50];
		sprintf_s(startText, sizeof(startText), "Press Space or Up or Down to start");
		lpFont->DrawText(NULL, startText, -1, &rcStart, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 128, 128, 128));

	}
	else
	{
		// 描画開始
		lpSprite->Begin(D3DXSPRITE_ALPHABLEND);


		//render background, has to be before player and enemy cause or else the background will be at the top and won't be able to see player and enemy
		RECT rcGround = { 2, 104, 2404, 130 };
		D3DXMatrixTranslation(&groundMat, groundX, groundY, 0);
		lpSprite->SetTransform(&groundMat);
		lpSprite->Draw(playerTex, &rcGround, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

		//render second background and loop it to make a moving background
		D3DXMatrixTranslation(&groundMat, groundX - 2404, groundY, 0);
		lpSprite->SetTransform(&groundMat);
		lpSprite->Draw(playerTex, &rcGround, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

		if (nightFlg == 0)
		{
			//render the clouds in the sky
			RECT rcCloud = { 166, 2, 258, 29 };
			for (int c = 0; c < cloudNum; c++)
			{
				if (cloudFlg[c] == 1)
				{
					D3DXMatrixTranslation(&cloudMat[c], cloudX[c], cloudY[c], 0);
					lpSprite->SetTransform(&cloudMat[c]);
					lpSprite->Draw(playerTex, &rcCloud, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
			}

			//render the 4 types of cactus enemy
			RECT rcEne[4] = { { 652, 2, 703, 98 },{ 652, 2, 750, 98 },{ 850, 2, 952, 102 },{ 802, 2, 952, 102 } };
			for (int e = 0; e < enemyNum; e++)
			{
				if (enemyFlg[e] == 1)
				{
					D3DXMatrixTranslation(&enemy1Mat, enemyX[e], enemyY, 0);
					lpSprite->SetTransform(&enemy1Mat);
					lpSprite->Draw(playerTex, &rcEne[e], &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
			}

			//render flying dinosaur
			for (int ef = 0; ef < enemyNum; ef++)
			{
				if (enemyFlyFlg[ef] == 1)
				{
					RECT rcFlyEnemy = { 259 + (enemyFlyAnime[ef] * 93), 2, 352 + (enemyFlyAnime[ef] * 93), 83 };
					D3DXMatrixTranslation(&enemyFlyMat[ef], enemyFlyX[ef], enemyFlyY[ef], 0);
					lpSprite->SetTransform(&enemyFlyMat[ef]);
					lpSprite->Draw(playerTex, &rcFlyEnemy, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
			}

			if (playerFlg == 1)
			{
				if (jumpFlg >= 1 && duckFlg == 0)
				{
					//render jumping player
					RECT rcJumpPlayer = { 1342, 2, 1422, 96 };
					D3DXMatrixTranslation(&playerMat, playerX, playerY, 0);
					lpSprite->SetTransform(&playerMat);
					lpSprite->Draw(playerTex, &rcJumpPlayer, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
				if (jumpFlg == 0 && duckFlg == 0)
				{
					//render running player
					RECT rcPlayer = { 1518 + (playerAnime * 88), 2, 1598 + (playerAnime * 88), 96 };
					D3DXMatrixTranslation(&playerMoveMat, playerX, playerY, 0);
					lpSprite->SetTransform(&playerMoveMat);
					lpSprite->Draw(playerTex, &rcPlayer, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
				if (duckFlg == 1)
				{
					//render player duck
					RECT rcDuckPlayer = { 1866 + (playerDuckAnime * 118), 36, 1984 + (playerDuckAnime * 118), 96 };
					D3DXMatrixTranslation(&playerDuckMat, playerX, playerY + 33, 0);
					lpSprite->SetTransform(&playerDuckMat);
					lpSprite->Draw(playerTex, &rcDuckPlayer, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
			}

			if (playerFlg == 0)
			{
				//render dead player
				RECT rcDeadPlayer = { 1694, 6, 1774, 92 };
				D3DXMatrixTranslation(&playerMat, playerX, playerY, 0);
				lpSprite->SetTransform(&playerMat);
				lpSprite->Draw(playerTex, &rcDeadPlayer, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

				//render game over word
				RECT rcGameOver = { 954, 29, 1336, 51 };
				D3DXMatrixTranslation(&gameOverMat, 440, 300, 0);
				lpSprite->SetTransform(&gameOverMat);
				lpSprite->Draw(playerTex, &rcGameOver, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

				//render replay button
				RECT rcReplay = { 4, 4, 72, 64 };
				D3DXMatrixTranslation(&replayMat, 585, 400, 0);
				lpSprite->SetTransform(&replayMat);
				lpSprite->Draw(playerTex, &rcReplay, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
		}
		else if (nightFlg == 1)
		{
			//render the clouds in the sky
			RECT rcCloud = { 166, 2, 258, 29 };
			for (int c = 0; c < cloudNum; c++)
			{
				if (cloudFlg[c] == 1)
				{
					D3DXMatrixTranslation(&cloudMat[c], cloudX[c], cloudY[c], 0);
					lpSprite->SetTransform(&cloudMat[c]);
					lpSprite->Draw(playerNightTex, &rcCloud, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
			}

			//render the 4 types of cactus enemy
			RECT rcEne[4] = { { 652, 2, 703, 98 },{ 652, 2, 750, 98 },{ 850, 2, 952, 102 },{ 802, 2, 952, 102 } };
			for (int e = 0; e < enemyNum; e++)
			{
				if (enemyFlg[e] == 1)
				{
					D3DXMatrixTranslation(&enemy1Mat, enemyX[e], enemyY, 0);
					lpSprite->SetTransform(&enemy1Mat);
					lpSprite->Draw(playerNightTex, &rcEne[e], &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
			}

			//render flying dinosaur
			for (int ef = 0; ef < enemyNum; ef++)
			{
				if ( enemyFlyFlg[ef] == 1)
				{
					RECT rcFlyEnemy = { 259 + (enemyFlyAnime[ef] * 93), 2, 352 + (enemyFlyAnime[ef] * 93), 83 };
					D3DXMatrixTranslation(&enemyFlyMat[ef], enemyFlyX[ef], enemyFlyY[ef], 0);
					lpSprite->SetTransform(&enemyFlyMat[ef]);
					lpSprite->Draw(playerNightTex, &rcFlyEnemy, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));				
				}	
			}

			if (playerFlg == 1)
			{
				if (jumpFlg >= 1 && duckFlg == 0)
				{
					//render jumping player
					RECT rcJumpPlayer = { 1342, 2, 1422, 96 };
					D3DXMatrixTranslation(&playerMat, playerX, playerY, 0);
					lpSprite->SetTransform(&playerMat);
					lpSprite->Draw(playerNightTex, &rcJumpPlayer, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
				if (jumpFlg == 0 && duckFlg == 0)
				{
					//render running player
					RECT rcPlayer = { 1518 + (playerAnime * 88), 2, 1598 + (playerAnime * 88), 96 };
					D3DXMatrixTranslation(&playerMoveMat, playerX, playerY, 0);
					lpSprite->SetTransform(&playerMoveMat);
					lpSprite->Draw(playerNightTex, &rcPlayer, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
				if (duckFlg == 1)
				{
					//render player duck
					RECT rcDuckPlayer = { 1866 + (playerDuckAnime * 118), 36, 1984 + (playerDuckAnime * 118), 96 };
					D3DXMatrixTranslation(&playerDuckMat, playerX, playerY + 30, 0);
					lpSprite->SetTransform(&playerDuckMat);
					lpSprite->Draw(playerNightTex, &rcDuckPlayer, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
			}

			if (playerFlg == 0)
			{
				//render dead player
				RECT rcDeadPlayer = { 1694, 6, 1774, 92 };
				D3DXMatrixTranslation(&playerMat, playerX, playerY, 0);
				lpSprite->SetTransform(&playerMat);
				lpSprite->Draw(playerNightTex, &rcDeadPlayer, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

				//render game over word
				RECT rcGameOver = { 954, 29, 1336, 51 };
				D3DXMatrixTranslation(&gameOverMat, 440, 300, 0);
				lpSprite->SetTransform(&gameOverMat);
				lpSprite->Draw(playerNightTex, &rcGameOver, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

				//render replay button
				RECT rcReplay = { 4, 4, 72, 64 };
				D3DXMatrixTranslation(&replayMat, 585, 400, 0);
				lpSprite->SetTransform(&replayMat);
				lpSprite->Draw(playerNightTex, &rcReplay, &D3DXVECTOR3(0, 0, 0), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
		}


		// 描画終了
		lpSprite->End();

		//文字表示はここから

		if (nightFlg == 0)
		{
			//text for highscore
			RECT rcScoreText = { 570, 0, 0, 0 };
			char scoreText[50];
			sprintf_s(scoreText, sizeof(scoreText), "HI %05d    score %05d", highScore, gameScore);
			lpFont->DrawText(NULL, scoreText, -1, &rcScoreText, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 128, 128, 128));
		}
		else if (nightFlg == 1)
		{
			//text for highscore
			RECT rcScoreText = { 570, 0, 0, 0 };
			char scoreText[50];
			sprintf_s(scoreText, sizeof(scoreText), "HI %05d    score %05d", highScore, gameScore);
			lpFont->DrawText(NULL, scoreText, -1, &rcScoreText, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 100, 100, 100));
		}
	}
}

void GameFrame(void)
{
	// バックバッファと Z バッファをクリア
	//this is where to change backgroung color
	if (nightFlg == 0)
	{
		lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
	}
	else if (nightFlg == 1)
	{
		lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	}


	// 更新処理
	Update();


	// 描画開始
	lpD3DDevice->BeginScene();

	D3DXMATRIX mView, mProj;

	// 視点行列の設定
	D3DXMatrixLookAtLH(&mView,
		&D3DXVECTOR3(0.0f, 0.0f, -10.0f),	// カメラの位置
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// カメラの視点
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f)	// カメラの頭の方向
	);

	// 投影行列の設定
	D3DXMatrixPerspectiveFovLH(&mProj, D3DXToRadian(60), (float)SCRW / (float)SCRH, 1.0f, 1000.0f);

	//行列設定
	lpD3DDevice->SetTransform(D3DTS_VIEW, &mView);
	lpD3DDevice->SetTransform(D3DTS_PROJECTION, &mProj);



	// 3D描画
	Render3D();

	// 2D描画
	Render2D();



	// 描画終了
	lpD3DDevice->EndScene();

	// バックバッファをプライマリバッファにコピー
	lpD3DDevice->Present(NULL, NULL, NULL, NULL);
}

LRESULT APIENTRY WndFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);

}

void LoadTexture(LPDIRECT3DTEXTURE9 *lpTex, const char fname[], int W, int H, D3DCOLOR Color)
{
	if (W == 0)W = D3DX_DEFAULT;
	if (H == 0)H = D3DX_DEFAULT;
	D3DXCreateTextureFromFileEx(lpD3DDevice, fname, W, H, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, Color, NULL, NULL, lpTex);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev,
	LPSTR lpszCmdParam, int nCmdshow)
{
	MSG msg;

	HWND hwnd;
	WNDCLASS wc;
	char szAppName[] = "Generic Game SDK Window";

	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WndFunc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,
		szAppName,
		"Direct X",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCRW, SCRH,
		NULL, NULL, hInst,
		NULL);

	if (!hwnd)return FALSE;

	ShowWindow(hwnd, nCmdshow);
	UpdateWindow(hwnd);
	SetFocus(hwnd);

	gameFullScreen = false;	// ウィンドウモード

	if (gameFullScreen) {
		ShowCursor(FALSE);
	}
	else {
		RECT rc = { 0,0,SCRW,SCRH };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		SetWindowPos(hwnd, NULL, 50, 50, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW | SWP_NOZORDER);
	}

	//---------------------DirectX Graphics関連-----------------------

	// Direct3D オブジェクトを作成
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!lpD3D)
	{
		// オブジェクト作成失敗
		MessageBox(NULL, "Direct3D の作成に失敗しました。", "ERROR", MB_OK | MB_ICONSTOP);
		// 終了する
		return 0;
	}
	int adapter;

	// 使用するアダプタ番号
	adapter = D3DADAPTER_DEFAULT;

	// ウインドウの作成が完了したので、Direct3D を初期化する
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	// Direct3D 初期化パラメータの設定
	if (gameFullScreen)
	{
		// フルスクリーン : ほとんどのアダプタでサポートされているフォーマットを使用
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	else
	{
		// ウインドウ : 現在の画面モードを使用
		D3DDISPLAYMODE disp;
		// 現在の画面モードを取得
		lpD3D->GetAdapterDisplayMode(adapter, &disp);
		d3dpp.BackBufferFormat = disp.Format;
	}
	// 表示領域サイズの設定
	d3dpp.BackBufferWidth = SCRW;
	d3dpp.BackBufferHeight = SCRH;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

	if (!gameFullScreen)
	{
		// ウインドウモード
		d3dpp.Windowed = 1;
	}

	// Z バッファの自動作成
	d3dpp.EnableAutoDepthStencil = 1;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	//ﾊﾞｯｸﾊﾞｯﾌｧをﾛｯｸ可能にする(GetDCも可能になる)
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	// デバイスの作成 - T&L HAL
	if (FAILED(lpD3D->CreateDevice(adapter, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &lpD3DDevice)))
	{
		// 失敗したので HAL で試行
		if (FAILED(lpD3D->CreateDevice(adapter, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &lpD3DDevice)))
		{
			// 失敗したので REF で試行
			if (FAILED(lpD3D->CreateDevice(adapter, D3DDEVTYPE_REF, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &lpD3DDevice)))
			{
				// 結局失敗した
				MessageBox(NULL, "DirectX9が初期化できません。\n未対応のパソコンと思われます。", "ERROR", MB_OK | MB_ICONSTOP);
				lpD3D->Release();
				// 終了する
				return 0;
			}
		}
	}

	// レンダリング・ステートを設定
	// Z バッファ有効化->前後関係の計算を正確にしてくれる
	lpD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// アルファブレンディング有効化
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// アルファブレンディング方法を設定
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// レンダリング時のアルファ値の計算方法の設定
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	// テクスチャの色を使用
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	// 頂点の色を使用
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	// レンダリング時の色の計算方法の設定
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	//裏面カリング
	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// フィルタ設定
	lpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	lpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	lpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// ライト
	lpD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	lpD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	// 頂点フォーマットの設定
	lpD3DDevice->SetFVF(FVF_VERTEX);

	timeBeginPeriod(1);

	// ゲームに関する初期化処理 ---------------------------

	//player texture
	LoadTexture(&playerTex, "dinoChrome.png", 2404, 130, D3DCOLOR_XRGB(247, 247, 247));
	LoadTexture(&playerNightTex, "dinoChromeNight.png", 2404, 130, D3DCOLOR_XRGB(0, 0, 0));
	/* LoadTexture(&playerTex, "dinoChrome.png", 2404, 130, D3DCOLOR_XRGB(255, 255, 255));   to change the background colour if needed*/

	srand(timeGetTime());

	GameReset();
	


	// スプライト作成
	D3DXCreateSprite(lpD3DDevice, &lpSprite);
	lpSprite->OnResetDevice();

	// フォント作成
	D3DXCreateFont(lpD3DDevice, 30/* 高さ */, 30/* 幅 */, FW_REGULAR, NULL, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "ＭＳ ゴシック", &lpFont);

	lpFont->OnResetDevice();

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			GameFrame();
		}
	}

	// ゲームに関する終了処理 ---------------------------

	//highscore
	if (playerFlg == 0)
	{
		if (gameScore > highScore)
		{
			highScore = gameScore;
		}
	}

	//release the memory for drawn sprite
	playerTex->Release();


	lpSprite->Release();	// スプライト
	lpFont->Release();		// フォント


	timeEndPeriod(1);

	// Direct3D オブジェクトを解放
	lpD3DDevice->Release();
	lpD3D->Release();

	return (int)msg.wParam;
}