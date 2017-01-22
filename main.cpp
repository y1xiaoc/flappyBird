#include <string>
#include <vector>
#include <sstream>
#include "cleanup.h"
#include "base.h"
#include "pipe.h"
#include "bird.h"

using namespace std;

int main(int, char**) {

	srand(time(NULL)); //用当前时间重置随机数种子

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { //初始化SDL基本库，失败则报错
		throw SDL_ERROR("init error: ");
	}
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { //初始化SDL_image库，失败则报错
		throw SDL_ERROR("init img error: ");
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { //初始化SDL_Mixer库，失败则报错
		throw SDL_ERROR("init mixer error: ");
	}
	if (TTF_Init() != 0) { //初始化SDL_TTF库，失败则报错
		throw SDL_ERROR("init ttf error: ");
	}

	//创建一个SDL窗口
	SDL_Window * win = SDL_CreateWindow("flappy Mi", 300, 50, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (win == nullptr) { //若创建失败则退出，并抛出异常
		SDL_Quit();
		throw SDL_ERROR("create window error: ");
	}

	//创建SDL的渲染器
	SDL_Renderer * ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) { //若创建失败则删除前一步的窗口，退出并抛出异常
		cleanup(win);
		SDL_Quit();
		throw SDL_ERROR("create renderer error: ");
	}
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND); //将渲染器的模式设置为BLEND，以显示半透明图像

	SDL_Rect fill_screen = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }; //覆盖全窗口的矩形，用于之后闪白

	SDL_Texture * img_background = loadTexture(getResPath("background.png"), ren); //读取背景图片
	SDL_Texture * img_ground = loadTexture(getResPath("ground.png"), ren); //读取地面图片
	SDL_Texture * img_pipe = loadTexture(getResPath("pipe.png"), ren); //读取管道图片
	SDL_Texture * img_board = loadTexture(getResPath("board.png"), ren); //读取计分板图片

	SDL_Texture * img_bird1 = loadTexture(getResPath("bird.png"), ren); //读取鸟图片，飞行状态1
	SDL_Texture * img_bird2 = loadTexture(getResPath("bird2.png"), ren); //读取鸟图片，飞行状态2
	SDL_Texture * img_bird3 = loadTexture(getResPath("bird3.png"), ren); //读取鸟图片，飞行状态3
	SDL_Texture * img_bird_d = loadTexture(getResPath("bird_d.png"), ren); //读取撞了障碍的鸟图片

	Mix_Chunk * snd_flap = loadSound(getResPath("flap.wav")); //读取扇翅的音效文件
	Mix_Chunk * snd_score = loadSound(getResPath("score.wav")); //读取得分的音效文件
	Mix_Chunk * snd_hit = loadSound(getResPath("hit.wav")); //读取撞障碍的音效文件
	Mix_Chunk * snd_bounce = loadSound(getResPath("bounce.wav")); //读取反弹的音效文件，用于bounce mode
	Mix_Chunk * snd_final = loadSound(getResPath("final.wav")); //读取计分板落下的音效文件

	TTF_Font * font = TTF_OpenFont(getResPath("Humor-Sans.ttf").c_str(), 50); //读取字体文件
	if (font == nullptr) { throw SDL_ERROR("load font error: "); } //若读取失败则报错
	
	stringstream ss; //初始化字符串流用于之后将分数转为字符串
	SDL_Texture * txt_score = nullptr; //初始化得分的文字贴图的指针
	SDL_Texture * txt_best = nullptr; //初始化最高分的文字贴图的指针
	SDL_Texture * txt_restart = createText("press any key to restart", font, { 0,0,0,255 }, ren); //创建restart文字贴图
	SDL_SetTextureBlendMode(txt_restart, SDL_BLENDMODE_BLEND); //将restart贴图设置为可透明，以便实现之后淡入效果
	SDL_Color WHITE = { 255,255,255,255 }; //定义颜色白色
	SDL_Color BLACK = { 0,0,0,255 }; //定义颜色黑色
	
	bird myBird(ren, img_bird1, img_bird2, img_bird3, img_bird_d, snd_flap, snd_hit, snd_bounce); //初始化小鸟，将需要的素材全部传入
	vector<pipe> pipeList = { pipe(ren, img_pipe, snd_score), pipe(ren, img_pipe, snd_score) }; //初始化管道的列表，因为一个屏幕最多只有两个管道同时出现，长度为2。将所需素材全部传入
	SDL_Event e; //初始化事件处理的对象，用于之后接收鼠标键盘操作等
	bool quit = false; //标志退出与否的变量
	unsigned ground_t = 0; //标志地面位置的变量，在之后若鸟没死则每帧加1，实现地面左移
	unsigned dying_t = 0; //标志死亡后帧数的变量，用来实现闪白和计分板掉落等动画效果
	int alpha, tWidth, tHeight; //文字的透明度，长宽等

	while (!quit) { //quit为假的时候循环将一直继续，关闭按钮或esc键会将quit设为真

		if (myBird.state == FLYING) { //若是飞翔状态，每帧的开头阶段做状态检测，决定是否产生水管，并检测是否撞到了水管
			if (ground_t % (600 / HORIZONTAL_SPEED) == 0) { //若地面运动到（600/水平速度）的整数倍，则产生第一个水管
				pipeList[0].init();
			}
			if (ground_t % (600 / HORIZONTAL_SPEED) == (600 / HORIZONTAL_SPEED / 2)) { //若地面运动到（600/水平速度）的半整数倍则产生第二个水管
				pipeList[1].init();
			}
			for (auto&p : pipeList) { //对每个水管检查是否碰撞
				myBird.checkHit(p);
			}
		}

		while (SDL_PollEvent(&e)) { //进行事件处理，读取事件队列中的事件，逐个处理
			if (e.type == SDL_QUIT) { //若点了关闭键则将quit设置为真，循环将不会继续
				quit = true;
				break;
			}
			if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) { //键盘或鼠标按下的动作
				if (e.key.keysym.sym == SDLK_ESCAPE) { //若按esc键也是退出
					quit = true;
					break;
				}
				if (myBird.state == DYING) { //若是已经死掉的状态，则按任何键将回到开始阶段
					if (dying_t > 20) { //只在死后20帧之后才作出反应，避免刚死立刻按键，看不到分数等
						myBird.init(); //将鸟重置为开始状态，复活
						pipe::grade = 0; //将分数置零
						for (auto& p : pipeList) { //将所有管子移出屏幕
							p.X = -p.width;
						}
					}
				}
				else { //若是没死状态，按任何键都能使鸟扇翅
					if (e.key.keysym.sym == SDLK_b && myBird.state==START) { //若是处在开始阶段并且按了b，还会进入bounce(弹跳)模式，碰到管子的上下面和地面会弹起
						myBird.bounce = true;
					}
					myBird.flap();
				}
			}
		}
		
		myBird.fall(); //每帧循环调用下落函数，更新鸟的位置和角度

		//计算部分结束，准备开始渲染部分
		SDL_RenderClear(ren); //清屏
		renderTexture(img_background, ren, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT * 7 / 8); //画背景，留出地面的位置

		for (auto& p : pipeList) { //画管子
			if (myBird.state == FLYING) { //若是飞翔状态，管子先移动（其实是鸟移动）再画
				p.move();
			}
			p.render(); //画管子
		}

		for (int i = 0; i < 35; ++i) { //画地面，因为是用很多小贴图拼出来的，需要循环35次，每次算出一块贴图的位置画出，其中引入ground_t变量用于控制地面移动
			int pos = (i*SCREEN_WIDTH / 20 - ground_t*HORIZONTAL_SPEED) % (SCREEN_WIDTH * 21 / 20) - SCREEN_WIDTH / 20;
			renderTexture(img_ground, ren, pos, SCREEN_HEIGHT * 7 / 8, SCREEN_WIDTH / 20, SCREEN_HEIGHT / 8);
		}

		myBird.render(); //画鸟

		if (myBird.state == DYING) { //若鸟已经是死亡状态，闪白并画计分板和restart提示
			//闪白
			alpha = dying_t > 20 ? 85 : (255 * (30 - dying_t) / 30); //通过alpha值控制透明度，从全白不透明逐渐降低到半透明
			SDL_SetRenderDrawColor(ren, 255, 255, 255, alpha); //先把渲染器的画笔颜色改为白色，透明度为alpha
			SDL_RenderFillRect(ren, &fill_screen); //将整个屏幕涂色
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); //把画笔颜色改回来（其实也可以不改）

			//画掉下来的计分板
			int board_y = dying_t < 25 ? -int((dying_t - 5) * (dying_t - 41))/2 : 160; //利用dying_t做计分板掉下来的动画，算出计分板的纵坐标
			renderTexture(img_board, ren, (SCREEN_WIDTH - BOARD_WIDTH) / 2, board_y, BOARD_WIDTH, BOARD_HEIGHT); //画计分板

			//画两个分数
			TTF_SetFontStyle(font, TTF_STYLE_NORMAL); //设置字体模式为正常（非粗体）

			ss << pipe::grade; //将分数（整型）放入字符串流，以便转化为字符串
			txt_score = createText(ss.str(), font, WHITE, ren);	 //创建分数的文字贴图，白色
			SDL_QueryTexture(txt_score, NULL, NULL, &tWidth, &tHeight); //获取文字贴图的长宽
			renderTexture(txt_score, ren, (SCREEN_WIDTH - tWidth) / 2, board_y+47); //画分数
			cleanup(txt_score); //释放掉文字帖图
			ss.str(""); //清空字符串流

			ss << pipe::highest; //对最高分做同样处理
			txt_best = createText(ss.str(), font, WHITE, ren); //创建贴图
			SDL_QueryTexture(txt_best, NULL, NULL, &tWidth, &tHeight); //获取长宽
			renderTexture(txt_best, ren, (SCREEN_WIDTH - tWidth) / 2, board_y + 117); //画最高分
			cleanup(txt_best); //释放贴图
			ss.str(""); //清空流

			if (dying_t > 20) { //在50帧后，淡入显示restart提示文字。100帧后，文字循环变淡变深。通过alpha实现
				alpha = dying_t < 70 ? 255 * (dying_t - 20) / 50 : 255 * (0.6 + 0.4 * cos(double(dying_t - 70) / 30));
				SDL_SetTextureAlphaMod(txt_restart, alpha); //设置贴图透明度
				renderTexture(txt_restart, ren, 50, 400, SCREEN_WIDTH - 100, 30); //画图
			}

			if (dying_t == 10) { //第十帧播放计分板掉落音乐
				Mix_PlayChannel(-1, snd_final, 0);
			}

			dying_t++; //死后每帧时间加一
		}
		else {
			TTF_SetFontStyle(font, TTF_STYLE_BOLD); //设置字体模式为粗体，用于飞行时显示分数
			
			//通过先画一层黑色再画一层白色来实现字体描边
			ss << pipe::grade; //把分数放入流，以便转换为字符串
			TTF_SetFontOutline(font, 1 ); //设置字体边缘为1，用来画黑边
			txt_score = createText(ss.str(), font, BLACK, ren); //创建黑色贴图打底
			SDL_QueryTexture(txt_score, NULL, NULL, &tWidth, &tHeight); //获取贴图长宽
			renderTexture(txt_score, ren, (SCREEN_WIDTH - tWidth) / 2, 100); //画黑色分数打底
			cleanup(txt_score); //释放贴图

			TTF_SetFontOutline(font, 0); //设置字体边缘为0，画白色文字
			txt_score = createText(ss.str(), font, WHITE, ren); //创建白色文字贴图
			renderTexture(txt_score, ren, (SCREEN_WIDTH - tWidth) / 2, 100); //画白色文字在黑色之上，实现描边效果
			cleanup(txt_score); //释放贴图

			ss.str(""); //清空字符串流
			dying_t = 0; //未死，每次重置死亡计时为0
			ground_t++; //控制地面前移
		}

		SDL_RenderPresent(ren); //显示图像
		SDL_Delay(1); //延迟一毫秒，避免帧率过大

	}
	//循环结束，已经退出，善后工作
	cleanup(txt_best, txt_restart, txt_score, font); //释放字体和文字贴图
	cleanup(snd_final, snd_flap, snd_hit, snd_score); //释放音效文件
	cleanup(img_background, img_bird1, img_bird2, img_bird3, img_bird_d, img_pipe, img_ground, ren, win); //释放贴图，渲染器和窗口
	TTF_Quit(); //退出TTF模块
	IMG_Quit(); //退出图像模块
	Mix_Quit(); //退出音效模块
	SDL_Quit(); //退出主模块

	return 0;
}