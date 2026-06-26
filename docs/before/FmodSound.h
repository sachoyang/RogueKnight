#pragma once

// FOMD 비지니스 로직
#include <string>

int		AddSoundFile(std::string _FullPath, bool IsLoop = false);
int 	EffectPlay(int _SoundNum);
void	BGPlay(int _SoundNum);
void	VolumUp();
void	VolumDown();
void	BGStop();
void    StopSound(int _ChannelIndex);
void    StopAllEffects();
void	SetMasterVolume(float vol);
bool	IsPlaying(int _ChannelIndex);
void    Update();