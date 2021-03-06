#include "pch.h"

MainMenuState::MainMenuState(std::shared_ptr<GameData> data) : data_(std::move(data))
{
	data_->songsData.unloadSong();

	bg_.setTexture(data_->assets.getTexture("menu bg"));

	enteredText_.setFont(data_->assets.getFont("MAIN"));
	enteredText_.setCharacterSize(45);
	enteredText_.setFillColor(sf::Color::Black);
	enteredText_.setPosition(float(data_->window.getSize().x * 0.5), float(data_->window.getSize().y * 0.7));

	data_->backgroundMusic.setVolume(data_->settings.currentMusicVolume);

	if (data_->backgroundMusic.getStatus() == sf::Music::Stopped)
	{
		data_->backgroundMusic.play();
	}
}

void MainMenuState::handleInput()
{
	sf::Event event{};

	while (data_->window.pollEvent(event))
	{
		if (sf::Event::Closed == event.type)
		{
			data_->window.close();
		}
		if (event.type == sf::Event::TextEntered)
		{
			stringEntered_ += event.text.unicode;
		}
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete)
		{
			stringEntered_ = "";
		}
		if (event.type == sf::Event::KeyPressed && event.key.code == data_->settings.volumeDown)
		{
			data_->settings.currentMusicVolume -= 10.0f;
			if (data_->settings.currentMusicVolume < 0)
			{
				data_->settings.currentMusicVolume = 0;
			}
			data_->backgroundMusic.setVolume(data_->settings.currentMusicVolume);

		}
		if (event.type == sf::Event::KeyPressed && event.key.code == data_->settings.volumeUp)
		{
			data_->settings.currentMusicVolume += 10.0f;
			if (data_->settings.currentMusicVolume > 100)
			{
				data_->settings.currentMusicVolume = 100;
			}
			data_->backgroundMusic.setVolume(data_->settings.currentMusicVolume);
		}
	}
}

void MainMenuState::update(const float dt)
{
	enteredText_.setOrigin(enteredText_.getGlobalBounds().width / 2, enteredText_.getGlobalBounds().height / 2);
	enteredText_.setString(stringEntered_);

	if (data_->songsData.loadSong(stringEntered_) && !played_)
	{
		played_ = true;
		data_->backgroundMusic.stop();
		data_->transitionSound.play();
		data_->maschine.removeState();
		data_->maschine.addState(std::make_unique<OptionsState>(data_, stringEntered_), true);
	}
	if (stringEntered_ == "exit")
	{
		data_->window.close();
	}
}

void MainMenuState::draw()
{
	data_->window.clear();
	data_->window.draw(bg_);
	data_->window.draw(enteredText_);
	data_->window.display();
}