#pragma once

// 여기에 사용할 사운드 이름을 추가하세요.
enum SoundType {
	// --- 배경음 (BGM) ---

	SND_BGM_STAGE1,
	SND_BGM_FIGHT,
	SND_BGM_BOSS,
	SND_BGM_HK,
	SND_BGM_CLEAR,

	// --- 효과음 (SFX) ---
	SND_EFF_JUMP,
	SND_EFF_DASH,
	SND_EFF_ATTACK,
	SND_EFF_ATTACKUNHIT,
	SND_EFF_HIT,
	SND_EFT_CRAWLER,
	SND_EFT_FLYER,
	SND_EFF_MONSTER_DIE,
	SND_EFF_BOSS_ROAR,
	SND_EFF_BOSS_ATTACK1,
	SND_EFF_BOSS_ATTACK2,
	SND_EFF_BOSS_ROLL,
	SND_EFF_BOSS_ROLL_SKY,
	SND_EFF_HERO_DIE,
	SND_EFF_BOSS_DIE,
	// (항상 마지막에 둠)
	SND_END
};


class Sound
{

private:

	int m_SoundIDs[SND_END];

	SoundType m_CurBgmType;
	SoundType m_CurEffectType;

public:

	Sound();
	~Sound();
	// 싱글톤 인스턴스 반환
	static Sound* GetInstance() {
		static Sound instance;
		return &instance;
	}
	
	void Init();
	void Update();
	// 재생 함수
	void PlayBGM(SoundType type);   // 배경음 재생 (Loop)
	int PlayEffect(SoundType type);
	void StopEffect(SoundType type);
	bool IsPlaying(int channel);

	// 제어 함수
	void StopBGM();
	void VolUp();
	void VolDown();
	void StopSound(int channel);    // 특정 채널의 소리 끄기
	void SetMasterVolume(float vol);

};

//extern Sound sound;

#define SOUND Sound::GetInstance()