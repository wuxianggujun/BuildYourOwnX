#pragma once

#include <map>
#include <string>
#include <vector>
#include "Sprite.hpp"
#include "Globls.hpp"

class Graphics;

// 动画精灵类，继承自精灵类
class AnimatedSprite : public Sprite{
public:
	// 构造函数
	AnimatedSprite();
	// 带参数的构造函数
	AnimatedSprite(Graphics& graphics, const std::string& filePath, int sourceX, int sourceY,
		int width, int height,float posX,float posY,float timeToUpdate);

	// 播放动画
	void playAnimation(std::string animationName, bool loop = true);

	// 更新动画
	void update(int elapsedTime);
    // 绘制动画
	void draw(Graphics& graphics, int x, int y);
	// 设置动画
	virtual void setupAnimation();

protected:
	// 更新时间
	double _timeToUpdate;
	// 当前动画是否只播放一次
	bool _currentAnimationOnce;
	// 当前动画名称
	std::string _currentAnimation;

	// 添加动画
	void addAnimation(int frame, int x, int y,std::string name, int width, int height, Vector2 offset);

	// 重置动画
	void resetAnimation();
	// 停止动画
	void stopAnimation();

	// 设置可见性
	void setVisible(bool visible);

	// 动画播放完毕后的回调函数
	virtual void animationDone(std::string currentAnimation);

private:
	// 动画集合
	std::map<std::string, std::vector<SDL_Rect>> _animations;
    // 动画偏移量集合
	std::map<std::string, Vector2> _offsets;

	// 当前帧索引
	int _frameIndex;
	// 时间流逝
	double _timeElapsed{0.0};
	// 是否可见
	bool _visible;
};
