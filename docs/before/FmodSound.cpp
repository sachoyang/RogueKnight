#include "FMODENGIN/inc/fmod.hpp"
#pragma	comment (lib, "FMODENGIN/lib/fmodex64_vc.lib") // 1. 경로의 명시적 효과, 속성 창에 라이브러리 추가
using namespace FMOD;

#include "FmodSound.h"

// STL 컨테이너
#include <map>

// FMOD 비지니스 로직 클래스
class FmodSound
{
	System* m_pSystem;
	int			m_Index;
	Channel* m_pBGChannel;

	ChannelGroup* m_pMasterGroup;
	float       m_vloum;

public:
	std::map<std::string, int >	m_CheckList;
	std::map<int, Sound* >		m_SoundList;

	FmodSound()
	{
		System_Create(&m_pSystem);
		// init(채널 최대설정값, 초기화 시점 , 추가로 넣을 보조 드라이버 );
		m_pSystem->init(120, FMOD_INIT_NORMAL, 0);

		m_pSystem->getMasterChannelGroup(&m_pMasterGroup);

		m_Index = 0;
		m_vloum = 1.0f;
		m_pBGChannel = nullptr;
	}
	~FmodSound()
	{
		m_CheckList.clear();

		for (auto& Iter : m_SoundList)
		{
			Iter.second->release();
		}
		m_SoundList.clear();

		m_pSystem->release();
		m_pSystem->close();
	}

	// 사운드 추가 
	int	AddSoundFile(std::string _FullPath, bool _IsLoop)
	{
		auto Find = m_CheckList.find(_FullPath);

		if (Find != m_CheckList.end())
		{
			return Find->second;
		}

		Sound* pSound = nullptr;
		//int		Mode = FMOD_HARDWARE | (_IsLoop ? FMOD_LOOP_NORMAL | FMOD_DEFAULT : FMOD_LOOP_OFF);
		// ✅ [수정 후] 명확하게 옵션 지정 (FMOD_2D 추가, DEFAULT 제거)
		int Mode = FMOD_HARDWARE | FMOD_2D;
		if (_IsLoop)
			Mode |= FMOD_LOOP_NORMAL; // 반복 재생
		else
			Mode |= FMOD_LOOP_OFF;    // 1회 재생 (기본)
		// 사운드를 지정된 경로에서 두 번째 인자인 LOOP/ONCE 로 선택 해서 로딩
		m_pSystem->createSound(_FullPath.c_str(), Mode, 0, &pSound);

		if (pSound == nullptr)
		{
			return -1;
		}

		m_CheckList.insert(std::make_pair(_FullPath, m_Index));
		m_SoundList.insert(std::make_pair(m_Index, pSound));

		return m_Index++;
	}

	// 해당 채널이 재생 중인지 검사
	bool IsPlaying(int _ChannelIndex)
	{
		if (_ChannelIndex < 0) return false;

		Channel* pChannel = nullptr;
		m_pSystem->getChannel(_ChannelIndex, &pChannel);

		bool isPlaying = false;
		if (pChannel)
		{
			pChannel->isPlaying(&isPlaying);
		}
		return isPlaying;
	}

	// 이펙트 플레이
	//void EffectPlay(int _SoundNum)
	//{
	//	auto Find = m_SoundList.find(_SoundNum);

	//	// 만약 이펙트도 볼륨 조절 원하면 클래스 멤버로 빼야함
	//	Channel* pChannel = nullptr;

	//	// playSound(채널그룹, sound, 일시정지니?,채널);
	//	m_pSystem->playSound(FMOD_CHANNEL_FREE, Find->second, 0, &pChannel);
	//}
	int EffectPlay(int _SoundNum)
	{
		auto Find = m_SoundList.find(_SoundNum);
		if (Find == m_SoundList.end()) return -1;

		Channel* pChannel = nullptr;
		m_pSystem->playSound(FMOD_CHANNEL_FREE, Find->second, 0, &pChannel);

		// 채널 ID(Index)를 뽑아서 리턴
		int channelIndex = -1;
		if (pChannel)
		{
			pChannel->getIndex(&channelIndex);
		}
		return channelIndex;
	}
	// 배경음 플레이
	void BGPlay(int _SoundNum)
	{
		auto Find = m_SoundList.find(_SoundNum);

		m_pSystem->playSound(FMOD_CHANNEL_REUSE, Find->second, 0, &m_pBGChannel);
	}

	// 배경음 멈춤
	void BGStop()
	{
		if (m_pBGChannel != nullptr)
		{
			m_pBGChannel->stop();
		}
	}

	// 볼륨 다운
	void VolumDown()
	{
		m_vloum -= 0.1f;
		if (m_vloum <= 0) m_vloum = 0;
		m_pBGChannel->setVolume(m_vloum);
	}

	// 볼륨 업
	void VolumUp()
	{
		m_vloum += 0.1f;
		if (m_vloum >= 1.0) m_vloum = 1.0;
		m_pBGChannel->setVolume(m_vloum);
	}

	void StopSound(int _ChannelIndex)
	{
		if (_ChannelIndex < 0) return;

		Channel* pChannel = nullptr;
		m_pSystem->getChannel(_ChannelIndex, &pChannel);

		if (pChannel)
		{
			pChannel->stop();
		}
	}
	void StopAllEffects()
	{
		// 간단한 방법: 마스터 채널 그룹 하위의 모든 소리 정지
		if (m_pMasterGroup)
		{
			m_pMasterGroup->stop();
			// 주의: 이렇게 하면 BGM도 꺼집니다. 
			// BGM은 ChangeMap에서 다시 PlayBGM하므로 괜찮지만,
			// 만약 효과음만 끄고 싶다면 채널 리스트를 관리해야 합니다.
		}
	}
	void SetMasterVolume(float vol)
	{
		m_vloum = vol;
		if (m_vloum < 0.0f) m_vloum = 0.0f;
		if (m_vloum > 1.0f) m_vloum = 1.0f;

		if (m_pMasterGroup)
		{
			m_pMasterGroup->setVolume(m_vloum);
		}
	}
	void Update()
	{
		if (m_pSystem)
		{
			m_pSystem->update(); // 🌟 FMOD 엔진 갱신 (필수!)
		}
	}
};

static FmodSound g_SoundMgr;

// 전역 사운드 추가 함수
int	AddSoundFile(std::string _FullPath, bool _IsLoop)
{
	return g_SoundMgr.AddSoundFile(_FullPath, _IsLoop);
}

// 전역 이펙트 플레이 함수
//void EffectPlay(int _SoundNum)
//{
//	g_SoundMgr.EffectPlay(_SoundNum);
//}
int EffectPlay(int _SoundNum)
{
	return g_SoundMgr.EffectPlay(_SoundNum);
}
// 전역 배경음 출력 함수
void BGPlay(int _SoundNum)
{
	g_SoundMgr.BGPlay(_SoundNum);
}

// 전역 볼륨 업
void VolumUp()
{
	g_SoundMgr.VolumUp();
}

// 전역 볼륨 다운
void VolumDown()
{
	g_SoundMgr.VolumDown();
}

// 전역 배경음 중지 함수
void BGStop()
{
	g_SoundMgr.BGStop();
}

void StopSound(int _ChannelIndex)
{
	g_SoundMgr.StopSound(_ChannelIndex);
}

void SetMasterVolume(float vol)
{
	g_SoundMgr.SetMasterVolume(vol);
}

void StopAllEffects()
{
	g_SoundMgr.StopAllEffects();
}

bool IsPlaying(int _ChannelIndex)
{
	return g_SoundMgr.IsPlaying(_ChannelIndex);
}

void Update()
{
	g_SoundMgr.Update();
}