#include "pch.h"
#include <iostream>

GameState::GameState(GameDataRef data, std::string songName) : data_(std::move(data)), songName_(std::move(songName))
{
	genDots_();

	bg_.setTexture(data_->assets.getTexture("game bg"));

	firstHitter_.setPosition(float(WIN_RES.x * 0.1188), float(WIN_RES.y * 0.8));
	secondHitter_.setPosition(float(WIN_RES.x * 0.282), float(WIN_RES.y * 0.8));
	thirdHitter_.setPosition(float(WIN_RES.x * 0.445), float(WIN_RES.y * 0.8));
	fourthHitter_.setPosition(float(WIN_RES.x * 0.607), float(WIN_RES.y * 0.8));

	scoreText_.setFont(data_->assets.getFont("MAIN"));
	scoreText_.setOrigin(scoreText_.getGlobalBounds().width, scoreText_.getGlobalBounds().height);
	scoreText_.setPosition(float(WIN_RES.x * 0.995), float(WIN_RES.y * 0.028));
	scoreText_.setFillColor(sf::Color::White);
	scoreText_.setCharacterSize(15);

	comboText_.setFont(data_->assets.getFont("MAIN"));
	comboText_.setPosition(float(WIN_RES.x * 0.995), float(WIN_RES.y * 0.07));
	comboText_.setFillColor(sf::Color::White);
	comboText_.setCharacterSize(15);

	hitSound_.setBuffer(data_->songsData.getSong(songName_).hitSound);
	hitSound_.setVolume(10);

	scrollSpeed_ = 5;
	musicDuration_ = data_->songsData.getSong(songName_).music.getDuration().asSeconds();

	data_->songsData.getSong(songName_).music.play();

	gameClock_.restart();
	songClock_.restart();
}

void GameState::handleInput()
{
	sf::Event event{};

	while (data_->window.pollEvent(event))
	{
		if (sf::Event::Closed == event.type)
		{
			data_->window.close();
		}

		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)
		{
			data_->songsData.getSong(songName_).music.stop();
			data_->maschine.addState(std::make_unique<PauseState>(data_, songName_, score_, maxCombo_), true);
		}

		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Hyphen)
		{
			currentVolume_ -= 10.0f;
			if (currentVolume_ < 0)
			{
				currentVolume_ = 0;
			}
			data_->songsData.getSong(songName_).music.setVolume(currentVolume_);

		}
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Equal)
		{
			currentVolume_ += 10.0f;
			if (currentVolume_ > 100)
			{
				currentVolume_ = 100;
			}
			data_->songsData.getSong(songName_).music.setVolume(currentVolume_);
		}

		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::LBracket)
		{
			scrollSpeed_--;
			if (scrollSpeed_ < 1)
			{
				scrollSpeed_ = 1;
			}
		}
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::RBracket)
		{
			scrollSpeed_++;
			std::cout << scrollSpeed_ << std::endl;
			if (scrollSpeed_ > 20)
			{
				scrollSpeed_ = 20;
			}
		}
	}
}

void GameState::update(float dt)
{
	if (gameClock_.getElapsedTime().asSeconds() > musicDuration_)
	{
		data_->maschine.addState(std::make_unique<EndGameState>(data_, songName_, score_, maxCombo_), true);
	}

	animateHitmarkers_();
	updateDots_();
	updateScore_();
}

void GameState::draw(float dt)
{
	data_->window.clear();

	data_->window.draw(bg_);
	data_->window.draw(scoreText_);
	data_->window.draw(comboText_);
	data_->window.draw(firstHitter_);
	data_->window.draw(secondHitter_);
	data_->window.draw(fourthHitter_);
	data_->window.draw(thirdHitter_);
	drawDots_();

	data_->window.display();
}

void GameState::updateScore_()
{
	if (!chart_.empty())
	{
		for (auto& dots : onScreen_)
		{
			bool failed = false;
			int currentScore = 0;
			if (InputManager::scoreCollision(dots[0].sprite, firstHitter_) && !dots[0].isHit && !dots[0].hasEmptyTex() && sf::Keyboard::isKeyPressed(data_->settings.hit1))
			{
				dots[0].isHit = true;
				dots[0].sprite.setTexture(data_->assets.getTexture("EMPTYTEX"));
				combo_++;
				currentScore += 5 + combo_;
			}
			else if (InputManager::scoreCollision(dots[0].sprite, firstHitter_) && !dots[0].isHit && dots[0].hasEmptyTex() && sf::Keyboard::isKeyPressed(data_->settings.hit1))
			{
				failed = true;
			}
			if (InputManager::scoreCollision(dots[1].sprite, secondHitter_) && !dots[1].isHit && !dots[1].hasEmptyTex() && sf::Keyboard::isKeyPressed(data_->settings.hit2))
			{
				dots[1].isHit = true;
				dots[1].sprite.setTexture(data_->assets.getTexture("EMPTYTEX"));
				combo_++;
				currentScore += 5 + combo_;
			}
			else if (InputManager::scoreCollision(dots[1].sprite, secondHitter_) && !dots[1].isHit && dots[1].hasEmptyTex() && sf::Keyboard::isKeyPressed(data_->settings.hit2))
			{
				failed = true;
			}
			if (InputManager::scoreCollision(dots[2].sprite, thirdHitter_) && !dots[2].isHit && !dots[2].hasEmptyTex() && sf::Keyboard::isKeyPressed(data_->settings.hit3))
			{
				dots[2].isHit = true;
				dots[2].sprite.setTexture(data_->assets.getTexture("EMPTYTEX"));
				combo_++;
				currentScore += 5 + combo_;
			}
			else if (InputManager::scoreCollision(dots[2].sprite, thirdHitter_) && !dots[2].isHit && dots[2].hasEmptyTex() && sf::Keyboard::isKeyPressed(data_->settings.hit3))
			{
				failed = true;
			}
			if (InputManager::scoreCollision(dots[3].sprite, fourthHitter_) && !dots[3].isHit && !dots[3].hasEmptyTex() && sf::Keyboard::isKeyPressed(data_->settings.hit4))
			{
				dots[3].isHit = true;
				dots[3].sprite.setTexture(data_->assets.getTexture("EMPTYTEX"));
				combo_++;
				currentScore += 5 + combo_;
			}
			else if (InputManager::scoreCollision(dots[3].sprite, fourthHitter_) && !dots[3].isHit && dots[3].hasEmptyTex() && sf::Keyboard::isKeyPressed(data_->settings.hit4))
			{
				failed = true;
			}

			if (failed)
			{
				combo_ = 0;
			}
			else
			{
				score_ += currentScore;
			}
			break;
		}
	}

	scoreText_.setString(std::to_string(score_));
	scoreText_.setOrigin(scoreText_.getGlobalBounds().width, scoreText_.getGlobalBounds().height);

	comboText_.setString(std::to_string(combo_) + "x");
	comboText_.setOrigin(comboText_.getGlobalBounds().width, comboText_.getGlobalBounds().height);

	if (combo_ > maxCombo_)
	{
		maxCombo_ = combo_;
	}
}

void GameState::genDots_()
{
	for (unsigned i = 0; i < data_->songsData.getSong(songName_).chart.firstRow.size(); i++)
	{
		std::vector<Hitmarker> temp;

		if (data_->songsData.getSong(songName_).chart.firstRow[i] == true) {
			Hitmarker dot1(data_->assets.getTexture("dot 1"));
			dot1.sprite.setPosition(float(WIN_RES.x * 0.1188), -dot1.sprite.getGlobalBounds().height);
			temp.emplace_back(dot1);
		}
		else
		{
			Hitmarker dot1(this->data_->assets.getTexture("EMPTYTEX"));
			dot1.sprite.setPosition(float(WIN_RES.x * 0.1188), -dot1.sprite.getGlobalBounds().height);
			temp.emplace_back(dot1);
		}

		if (data_->songsData.getSong(songName_).chart.secondRow[i] == true) {
			Hitmarker dot2(data_->assets.getTexture("dot 2"));
			dot2.sprite.setPosition(float(WIN_RES.x * 0.282), -dot2.sprite.getGlobalBounds().height);
			temp.emplace_back(dot2);
		}
		else
		{
			Hitmarker dot2(this->data_->assets.getTexture("EMPTYTEX"));
			dot2.sprite.setPosition(float(WIN_RES.x * 0.282), -dot2.sprite.getGlobalBounds().height);
			temp.emplace_back(dot2);
		}

		if (data_->songsData.getSong(songName_).chart.thirdRow[i] == true) {
			Hitmarker dot3(data_->assets.getTexture("dot 3"));
			dot3.sprite.setPosition(float(WIN_RES.x * 0.445), -dot3.sprite.getGlobalBounds().height);
			temp.emplace_back(dot3);
		}
		else
		{
			Hitmarker dot3(this->data_->assets.getTexture("EMPTYTEX"));
			dot3.sprite.setPosition(float(WIN_RES.x * 0.445), -dot3.sprite.getGlobalBounds().height);
			temp.emplace_back(dot3);
		}

		if (data_->songsData.getSong(songName_).chart.fourthRow[0] == true) {
			Hitmarker dot4(data_->assets.getTexture("dot 4"));
			dot4.sprite.setPosition(float(WIN_RES.x * 0.607), -dot4.sprite.getGlobalBounds().height);
			temp.emplace_back(dot4);
		}
		else
		{
			Hitmarker dot4(this->data_->assets.getTexture("EMPTYTEX"));
			dot4.sprite.setPosition(float(WIN_RES.x * 0.607), -dot4.sprite.getGlobalBounds().height);
			temp.emplace_back(dot4);
		}

		chart_.emplace_back(temp);
	}
}


void GameState::drawDots_()
{
	for (const auto& i : onScreen_)
	{
		for (const auto& j : i)
		{
			data_->window.draw(j.sprite);
		}
	}
}

void GameState::updateDots_()
{
	if (songClock_.getElapsedTime().asSeconds() > data_->songsData.getSong(songName_).beatDuration && !chart_.empty())
	{
		songClock_.restart();
		onScreen_.emplace_back(chart_[0]);
		chart_.erase(chart_.begin());
	}

	for (auto& i : onScreen_)
	{
		for (auto& j : i)
		{
			j.sprite.move(0, scrollSpeed_);
			if (j.sprite.getPosition().y > WIN_RES.y)
			{
				for (const auto& dot : *onScreen_.begin())
				{
					if (!dot.hasEmptyTex())
					{
						combo_ = 0;
						break;
					}
				}
				onScreen_.erase(onScreen_.begin());
				break;
			}
		}
	}
}

void GameState::animateHitmarkers_()
{
	if (sf::Keyboard::isKeyPressed(data_->settings.hit1))
	{
		firstHitter_.setTexture(data_->assets.getTexture("hit 1 on"));
	}
	else
	{
		firstHitter_.setTexture(data_->assets.getTexture("hit off"));
	}

	if (sf::Keyboard::isKeyPressed(data_->settings.hit2))
	{
		secondHitter_.setTexture(data_->assets.getTexture("hit 2 on"));
	}
	else
	{
		secondHitter_.setTexture(data_->assets.getTexture("hit off"));
	}

	if (sf::Keyboard::isKeyPressed(data_->settings.hit3))
	{
		thirdHitter_.setTexture(data_->assets.getTexture("hit 3 on"));
	}
	else
	{
		thirdHitter_.setTexture(data_->assets.getTexture("hit off"));
	}

	if (sf::Keyboard::isKeyPressed(data_->settings.hit4))
	{
		fourthHitter_.setTexture(data_->assets.getTexture("hit 4 on"));
	}
	else
	{
		fourthHitter_.setTexture(data_->assets.getTexture("hit off"));
	}
}