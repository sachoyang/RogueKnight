#include "Include.h"

Sound sound;



Sound::Sound()
{
	// ID 배열 초기화
	for (int i = 0; i < SND_END; i++)
		m_SoundIDs[i] = -1;

	m_CurBgmType = (SoundType)-1;
	m_CurEffectType = (SoundType)-1;
}

Sound::~Sound()
{
}

void Sound::Init()
{
	//VolDown();
	//VolDown();
	//g_pSoundManager->Create( &m_Bk1, (LPWSTR)L"./resource/Sound/BGM_0001.wav", 0, GUID_NULL ) ;
	//g_pSoundManager->Create( &m_Shot, (LPWSTR)L"./resource/Sound/shot.wav", 0, GUID_NULL ) ;

	// =========================================================
	// AddSoundFile("경로", 반복여부)
	// =========================================================

	// 1. 배경음 (반복 = true)
	m_SoundIDs[SND_BGM_STAGE1] = AddSoundFile("./resource/Sound/RESTING GROUNDS.wav", true);
	m_SoundIDs[SND_BGM_FIGHT] = AddSoundFile("./resource/Sound/Stage_Theme.wav", true);
	m_SoundIDs[SND_BGM_BOSS] = AddSoundFile("./resource/Sound/watcher/Boss Battle 1.wav", true);
	m_SoundIDs[SND_BGM_HK] = AddSoundFile("./resource/Sound/Hollow Knight.wav", true);
	m_SoundIDs[SND_BGM_CLEAR] = AddSoundFile("./resource/Sound/Clear.wav", false);

	// 2. 효과음 (반복 = false)
	m_SoundIDs[SND_EFF_JUMP] = AddSoundFile("./resource/Sound/hero sound/Jump.wav", false);
	m_SoundIDs[SND_EFF_DASH] = AddSoundFile("./resource/Sound/hero sound/Dash.wav", false);
	m_SoundIDs[SND_EFF_ATTACK] = AddSoundFile("./resource/Sound/hero sound/Attack.wav", false);
	m_SoundIDs[SND_EFF_ATTACKUNHIT] = AddSoundFile("./resource/Sound/hero sound/Unhit.wav", false);
	m_SoundIDs[SND_EFF_HIT] = AddSoundFile("./resource/Sound/hero sound/Hit.wav", false);
	m_SoundIDs[SND_EFT_CRAWLER] = AddSoundFile("./resource/Sound/crawler.wav", true);
	m_SoundIDs[SND_EFT_FLYER] = AddSoundFile("./resource/Sound/mosquito_fly_loop.wav", true);
	m_SoundIDs[SND_EFF_MONSTER_DIE] = AddSoundFile("./resource/Sound/MonsterDie.wav", false);
	m_SoundIDs[SND_EFF_BOSS_ROAR] = AddSoundFile("./resource/Sound/watcher/boss_explode.wav", false);
	m_SoundIDs[SND_EFF_BOSS_ROLL] = AddSoundFile("./resource/Sound/watcher/spinground2.wav", true);
	m_SoundIDs[SND_EFF_BOSS_ROLL_SKY] = AddSoundFile("./resource/Sound/watcher/spinair3.wav", true);
	m_SoundIDs[SND_EFF_HERO_DIE] = AddSoundFile("./resource/Sound/watcher/Boss Defeat.wav", false);
	m_SoundIDs[SND_EFF_BOSS_DIE] = AddSoundFile("./resource/Sound/boss_explode.wav", false);
	m_SoundIDs[SND_EFF_BOSS_ATTACK1] = AddSoundFile("./resource/Sound/watcher/BossAttack.wav", false);
	m_SoundIDs[SND_EFF_BOSS_ATTACK2] = AddSoundFile("./resource/Sound/watcher/BossAttack.wav", false);

}

void Sound::PlayBGM(SoundType type)
{
	// 유효한 범위인지, 로드가 제대로 됐는지 체크
	if (type < 0 || type >= SND_END || m_SoundIDs[type] == -1) return;

	if (m_CurBgmType == type)
	{
		return; // 그냥 함수 종료 (재생 안 함 -> 끊김 없이 계속 나옴)
	}

	// 다른 노래라면? -> 현재 노래 갱신하고 재생!
	m_CurBgmType = type;

	// FmodSound의 전역 함수 호출 (새로운 노래 틀기)
	BGPlay(m_SoundIDs[type]);
}

int Sound::PlayEffect(SoundType type)
{
	if (type < 0 || type >= SND_END || m_SoundIDs[type] == -1) return -1;

	// 다른 노래라면? -> 현재 노래 갱신하고 재생!
	m_CurEffectType = type;

	// FmodSound의 전역 함수 호출
	return EffectPlay(m_SoundIDs[type]);
}

void Sound::StopBGM()
{
	BGStop();
	m_CurBgmType = (SoundType)-1;
}

void Sound::VolUp()
{
	VolumUp();
}

void Sound::VolDown()
{
	VolumDown();
}

void Sound::StopSound(int channel)
{
	// 전역 함수 호출 (FmodSound.h에 선언된 것)
	::StopSound(channel);
}

void Sound::StopEffect(SoundType type)
{
	// 예외 처리
	if (type < 0 || type >= SND_END || m_SoundIDs[type] == -1) return;

}
void Sound::SetMasterVolume(float vol)
{
	::SetMasterVolume(vol); // FmodSound 전역 함수 호출
}

bool Sound::IsPlaying(int channel)
{
	return ::IsPlaying(channel); // 전역 함수 호출
}
void Sound::Update()
{
	::Update(); // FmodSound의 전역 함수 호출
}
// 사용법
//배경음 재생(MapManager::ChangeMap 등)
//
//C++
//// 1스테이지 BGM 틀기 (기존 BGM은 알아서 꺼짐)
//SOUND->PlayBGM(SND_BGM_STAGE1);
//점프할 때(Player::Update)
//
//C++
//if (key.KeyDown(VK_SPACE))
//{
//	// 점프 로직...
//	SOUND->PlayEffect(SND_EFF_JUMP); // 점프 소리!
//}
//공격할 때
//
//C++
//SOUND->PlayEffect(SND_EFF_ATTACK);
//보스방 입장 시
//
//C++
//SOUND->PlayBGM(SND_BGM_BOSS); // 보스 음악으로 전환
//SOUND->PlayEffect(SND_EFF_BOSS_ROAR); // 보스 포효 효과음