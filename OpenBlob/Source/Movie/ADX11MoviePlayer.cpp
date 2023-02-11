#include "pch.h"
#include "ADX11MoviePlayer.h"
#include "Platform/Windows/DX11/TRender_DX11.h"
#include "Toshi/Sound/TSound.h"

FMOD_RESULT F_CALLBACK pcmreadcallback(FMOD_SOUND* sound, void* data, unsigned int datalen)
{
    // Read from your buffer here...
    ADX11MoviePlayer* moviePlayer;
    FMOD_Sound_GetUserData(sound, (void**)&moviePlayer);
    moviePlayer->ReadBuffer(data, datalen);
    return FMOD_OK;
}


FMOD_RESULT F_CALLBACK pcmsetposcallback(FMOD_SOUND* sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    // Seek to a location in your data, may not be required for what you want to do
    return FMOD_OK;
}

ADX11MoviePlayer::ADX11MoviePlayer() : m_CurrentFileName("")
{
    m_TheoraDecoder = TNULL;
    m_TheoraVideo = TNULL;
    m_TheoraAudio = TNULL;
    m_pFile = TNULL;
    m_TexturesWidth = 0;
    m_TexturesHeight = 0;
    m_iTexIndex = 0;
    m_Textures[0] = TNULL;
    m_Textures[1] = TNULL;
    m_Textures[2] = TNULL;
    m_Textures[3] = TNULL;
    m_Textures[4] = TNULL;
    m_Textures[5] = TNULL;
    m_pPixelShader = TNULL;
    m_pVertexShader = TNULL;
    m_pBuffer = TNULL;
    m_pInputLayout = TNULL;
    m_FrameMS = 0;
    m_bIsPlaying = TFALSE;
    m_bIsPaused = TTRUE;
    m_AudioOffset = 0;
}

void ADX11MoviePlayer::PlayMovie(const char* fileName, void* unused, uint8_t flags)
{
    if (IsMoviePlaying())
    {
        StopMovieImpl();
        Toshi::TUtil::MemClear(m_CurrentFileName, MAX_FILE_NAME);
    }

    if (fileName == TNULL)
    {
        return;
    }

    char path[256];
    Toshi::T2String8::CopySafe(m_CurrentFileName, fileName, MAX_FILE_NAME);
    Toshi::T2String8::Format(path, "Data\\Movies\\%s.bik.ogv", fileName);
    m_pFile = fopen(path, "rb");

    if (m_pFile == TNULL)
    {
        m_pFile = fopen(m_CurrentFileName, "rb");
    }

    if (m_pFile != TNULL)
    {
        m_TheoraDecoder = THEORAPLAY_startDecodeFilePtr(m_pFile, 30, THEORAPLAY_VIDFMT_IYUV);

        if (m_TheoraDecoder != TNULL)
        {
            const THEORAPLAY_VideoFrame* video = NULL;
            const THEORAPLAY_AudioPacket* audio = NULL;

            while (!THEORAPLAY_isInitialized(m_TheoraDecoder));

            if (!THEORAPLAY_hasVideoStream(m_TheoraDecoder))
            {
                THEORAPLAY_stopDecode(m_TheoraDecoder);
                m_TheoraDecoder = NULL;
                return;
            }

            m_bHasAudioStream = THEORAPLAY_hasAudioStream(m_TheoraDecoder);

            while ((video = THEORAPLAY_getVideo(m_TheoraDecoder)) == NULL);
            while ((audio = THEORAPLAY_getAudio(m_TheoraDecoder)) == NULL)
            {
                if (THEORAPLAY_availableVideo(m_TheoraDecoder) >= 30) break;
            }

            UINT framems = (video->fps == 0.0) ? 0 : ((UINT)(1000.0 / video->fps));
            m_FrameMS = framems;

            if (m_TexturesWidth != video->width || m_TexturesHeight != video->height)
            {
                ReleaseTextures();
                CreateTextures(video->width, video->height);
                m_TexturesWidth = video->width;
                m_TexturesHeight = video->height;
            }

            if (m_bHasAudioStream)
            {
                FMOD::System* system = Toshi::TSound::GetSingletonWeak()->GetSystem();
                FMOD_CREATESOUNDEXINFO soundInfo;
                memset(&soundInfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
                soundInfo.numchannels = audio->channels;
                soundInfo.defaultfrequency = audio->freq;
                soundInfo.length = ~0U;
                soundInfo.format = FMOD_SOUND_FORMAT_PCMFLOAT;
                soundInfo.decodebuffersize = soundInfo.numchannels * soundInfo.defaultfrequency / 16;
                soundInfo.pcmreadcallback = pcmreadcallback;
                soundInfo.pcmsetposcallback = pcmsetposcallback;
                soundInfo.userdata = this;
                soundInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
                FMOD::Sound* sound;
                FMOD_RESULT eResult = system->createSound(NULL, 0x4CA, &soundInfo, &sound);
                if (eResult == FMOD_OK)
                {
                    FMOD_RESULT eResult = system->playSound(sound, NULL, true, &m_pChannel);
                    if (eResult != FMOD_OK)
                    {
                        m_pChannel = NULL;
                        sound->release();
                        sound = NULL;
                    }
                }
                if (m_pChannel != NULL)
                {
                    m_pChannel->setPriority(0);
                    TIMPLEMENT();
                    m_pChannel->setVolume(0.3f);
                    m_pChannel->setPaused(false);
                }
            }

            Mute(flags & 0b10);
            m_TheoraVideo = video;
            m_bIsPlaying = TTRUE;
            m_bIsPaused = TFALSE;
            m_bIsHidden = TFALSE;
            m_Position = 0;
        }
    }
}

void ADX11MoviePlayer::StopMovie()
{
    StopMovieImpl();
    ThrowPauseEvent(AMovieEvent::Type_Stopped);
}

void ADX11MoviePlayer::StopMovieImpl()
{
    if (m_bHasAudioStream && m_pChannel != TNULL)
    {
        m_bIsPlaying = false;
        FMOD::Sound* sound;
        m_pChannel->getCurrentSound(&sound);
        m_pChannel->stop();
        bool isPlaying;
        do
        {
            m_pChannel->isPlaying(&isPlaying);
        } while (isPlaying);
    }
    THEORAPLAY_stopDecode(m_TheoraDecoder);
    m_TheoraDecoder = TNULL;
    if (m_TheoraVideo != TNULL)
    {
        THEORAPLAY_freeVideo(m_TheoraVideo);
        m_TheoraVideo = TNULL;
    }
    if (m_TheoraAudio != TNULL)
    {
        THEORAPLAY_freeAudio(m_TheoraAudio);
        m_TheoraAudio = TNULL;
        m_AudioOffset = 0;
    }
    m_bIsPlaying = false;
    m_bIsHidden = true;
}

void ADX11MoviePlayer::PauseMovie(bool pause)
{
    if (pause != m_bIsPaused)
    {
        if (m_bIsPaused && m_pChannel != NULL)
        {
            m_pChannel->setPaused(pause);
        }
        m_bIsPaused = pause;
    }
}

bool ADX11MoviePlayer::IsMoviePlaying()
{
    return m_bIsPlaying;
}

bool ADX11MoviePlayer::IsMoviePaused()
{
    return m_bIsPaused;
}

void ADX11MoviePlayer::OnRender(float deltaTime)
{
    if (IsMoviePlaying())
    {
        if (!IsMoviePaused())
        {
            auto pRender = Toshi::TRenderDX11::Interface();
            auto video = m_TheoraVideo;

            if (video != TNULL)
            {
                UINT width = video->width;
                UINT height = video->height;
                UINT halfWidth = width >> 1;
                UINT pixelCount = width * height;
                UINT quarterPixelCount = halfWidth * (height >> 1);

                uint8_t* src = video->pixels + pixelCount;
                pRender->CopyDataToTexture(m_Textures[m_iTexIndex], pixelCount, video->pixels, width);
                pRender->CopyDataToTexture(m_Textures[m_iTexIndex + 2], quarterPixelCount, src, halfWidth);
                pRender->CopyDataToTexture(m_Textures[m_iTexIndex + 4], quarterPixelCount, src + quarterPixelCount, halfWidth);
                
                pRender->SetZMode(true, D3D11_COMPARISON_LESS_EQUAL, D3D11_DEPTH_WRITE_MASK_ALL);
                pRender->SetBlendMode(true, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE, D3D11_BLEND_ZERO);

                auto pDeviceContext = pRender->m_pDeviceContext;
                pDeviceContext->VSSetShader(m_pVertexShader, TNULL, 0);
                pDeviceContext->PSSetShader(m_pPixelShader, TNULL, 0);

                pDeviceContext->PSSetShaderResources(0, 1, &m_Textures[m_iTexIndex]);
                pRender->SetSamplerState(0, 1, 1);
                pDeviceContext->PSSetShaderResources(1, 1, &m_Textures[m_iTexIndex + 2]);
                pRender->SetSamplerState(1, 1, 1);
                pDeviceContext->PSSetShaderResources(2, 1, &m_Textures[m_iTexIndex + 4]);
                pRender->SetSamplerState(2, 1, 1);

                pDeviceContext->IASetInputLayout(m_pInputLayout);
                pRender->m_CurrentRasterizerId.Flags.Parts.CullMode = D3D11_CULL_NONE;
                pRender->DrawMesh(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, m_pBuffer, 4, 8, 0, 0);
            }
        }
    }
}

void ADX11MoviePlayer::OnUpdate(float deltaTime)
{
    if (IsMoviePlaying() && !IsMoviePaused())
    {
        TASSERT(m_TheoraDecoder && m_TheoraVideo);
        m_Position += deltaTime * 1000;

        if (m_bHasAudioStream && m_pChannel == TNULL)
        {
            const THEORAPLAY_AudioPacket* audio = THEORAPLAY_getAudio(m_TheoraDecoder);
            while (audio != NULL)
            {
                THEORAPLAY_freeAudio(audio);
                audio = THEORAPLAY_getAudio(m_TheoraDecoder);
            }
        }

        if (m_TheoraVideo->playms <= m_Position)
        {
            const THEORAPLAY_VideoFrame* last = m_TheoraVideo;
            while ((m_TheoraVideo = THEORAPLAY_getVideo(m_TheoraDecoder)) != NULL)
            {
                THEORAPLAY_freeVideo(last);
                last = m_TheoraVideo;

                if ((m_Position - m_TheoraVideo->playms) < m_FrameMS)
                {
                    break;
                }
            }

            if (!m_TheoraVideo)
            {
                m_TheoraVideo = last;
            }

            if (m_TheoraVideo == TNULL)
            {
                if (m_bIsMovieLooping)
                {
                    ThrowPauseEvent(AMovieEvent::Type_Looping);
                    uint8_t flags = 0;
                    if (m_bIsMuted)
                    {
                        flags = 2;
                    }
                    PlayMovie(m_CurrentFileName, TNULL, flags | m_bIsMovieLooping);
                }
                else
                {
                    StopMovieImpl();
                    ThrowPauseEvent(AMovieEvent::Type_Finished);
                }
            }
        }
    }

    AMoviePlayer::OnUpdate(deltaTime);
}

void ADX11MoviePlayer::OnCreate()
{
    CompileShader();
    THEORAPLAY_SetMemoryFunctions(TMalloc, TFree);
    AMoviePlayer::OnCreate();
}

void ADX11MoviePlayer::CreateTextures(UINT width, UINT height)
{
    auto pRender = Toshi::TRenderDX11::Interface();
    
    for (int i = 0; i < 2; i++)
    {
        m_Textures[i] = pRender->CreateTexture(width, height, DXGI_FORMAT_A8_UNORM, TNULL, 0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 1);
        m_Textures[i + 2] = pRender->CreateTexture(width / 2, height / 2, DXGI_FORMAT_A8_UNORM, TNULL, 0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 1);
        m_Textures[i + 4] = pRender->CreateTexture(width / 2, height / 2, DXGI_FORMAT_A8_UNORM, TNULL, 0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 1);
    }
}

void ADX11MoviePlayer::ReleaseTextures()
{
    for (int i = 0; i < 2; i++)
    {
        if (m_Textures[i] != TNULL)
        {
            m_Textures[i]->Release();
        }

        if (m_Textures[i + 2] != TNULL)
        {
            m_Textures[i + 2]->Release();
        }

        if (m_Textures[i + 4] != TNULL)
        {
            m_Textures[i + 4]->Release();
        }
    }
}

static FLOAT s_ScreenMesh[8] = {
    -1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, -1.0f,
    1.0f, 1.0f
};

void ADX11MoviePlayer::CompileShader()
{
    auto pRender = Toshi::TRenderDX11::Interface();
    m_pBuffer = pRender->CreateBuffer(0, sizeof(s_ScreenMesh), s_ScreenMesh, D3D11_USAGE_IMMUTABLE, 0);

    {
        // Pixel shader
        auto pBlob = pRender->CompileShader(
            "sampler2D YTexture : register( s0 );sampler2D UTexture : register( s1 );sampler2D VTexture : register( s2 );struct PS_IN{   float4 Position : POSITION; float2 UV : TEXCOORD0;};static const float3 offset = float3(-0.0625, -0.5, -0.5);static const float3 Rcoeff = float3(1.164, 0.000, 1.596);static const float3 Gcoeff = float3 (1.164, -0.391, -0.813);static const float3 Bcoeff = float3(1.164, 2.018, 0.000);float4 main( PS_IN In ) : COLOR{float3 yuv;yuv.r = tex2D( YTexture, In.UV ).a; yuv.g = tex2D( UTexture, In.UV ).a;yuv.b = tex2D( VTexture, In.UV ).a;yuv += offset;float3 rgb;rgb.r = dot(yuv, Rcoeff);rgb.g = dot(yuv, Gcoeff);rgb.b = dot(yuv, Bcoeff);return float4(rgb, 1.0);}",
            "main",
            "ps_4_0_level_9_3",
            0
        );

        pRender->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_pPixelShader);
        pBlob->Release();
    }

    {
        // Vertex shader
        auto pBlob = pRender->CompileShader(
            "struct VS_OUT { float4 Position : POSITION;   float2 UV : TEXCOORD0;};   VS_OUT main( float2 Position : POSITION ) { VS_OUT Out;   Out.Position = float4(Position, 0.0f, 1.0f);Out.UV = Position.xy * float2(0.5f, -0.5f) + 0.5f;   return Out;   }",
            "main",
            "vs_4_0_level_9_3",
            0
        );

        pRender->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_pVertexShader);
        
        D3D11_INPUT_ELEMENT_DESC inputDesc;
        inputDesc.SemanticName = "POSITION";
        inputDesc.SemanticIndex = 0;
        inputDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
        inputDesc.InputSlot = 0;
        inputDesc.AlignedByteOffset = -1;
        inputDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDesc.InstanceDataStepRate = 0;

        pRender->m_pDevice->CreateInputLayout(&inputDesc, 1, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_pInputLayout);
        pBlob->Release();
    }
}

void ADX11MoviePlayer::ReadBuffer(void* data, uint32_t datalen)
{
    if (datalen == 0) return;
    const THEORAPLAY_AudioPacket* audio;
    char* dataBuffer = (char*)data;

    while (m_bIsPlaying && datalen != 0)
    {
        audio = m_TheoraAudio;
        if (m_TheoraAudio == NULL && (!THEORAPLAY_isDecoding(m_TheoraDecoder) || !m_bIsPlaying || (audio = THEORAPLAY_getAudio(m_TheoraDecoder)) == NULL))
        {
            memset(dataBuffer, 0, datalen);
            return;
        }
        
        int channels = audio->channels;
        int size = (audio->frames - m_AudioOffset) * channels;
        
        int floatCount = datalen / sizeof(float);
        if (floatCount <= size && size - floatCount != 0)
        {
            size = floatCount;
        }

        memcpy(dataBuffer, audio->samples + (channels * m_AudioOffset), size * sizeof(float));
        dataBuffer += size * sizeof(float);
        datalen -= size * sizeof(float);

        m_AudioOffset += (size / channels);

        if (audio->frames <= m_AudioOffset)
        {
            THEORAPLAY_freeAudio(audio);
            audio = NULL;
            m_AudioOffset = 0;
        }

        m_TheoraAudio = audio;
    }
}
