#pragma once
#include "Singleton.h"
#include "Audio.h"
#include <memory>
#include <map>
#include "TypeHelpers.h"

class SoundHandler : public Singleton<SoundHandler>
{
	friend class Singleton<SoundHandler>;
public:
	SoundHandler() {};
	~SoundHandler();

	void Initialize();
	void Update();

	void LoadSFX(Name name, std::wstring filePath);
	void PlayOneShot(Name name);

	void PlayMusic(Name name);

	void SetVolume(Name name, float volume);


private:
	std::unique_ptr<DirectX::AudioEngine> audioEngine;

	std::map<Name, std::unique_ptr<DirectX::SoundEffect>> soundEffects;
	std::map<Name, std::unique_ptr<DirectX::SoundEffectInstance>> soundEffectInstances;


	
};

