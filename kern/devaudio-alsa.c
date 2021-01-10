/*
 * ALSA
 */
#include <alsa/asoundlib.h>
#include	"u.h"
#include	"lib.h"
#include	"dat.h"
#include	"fns.h"
#include	"error.h"
#include	"devaudio.h"

enum
{
	Channels = 2,
	Rate = 44100,
	Bits = 16,
};

static snd_pcm_t *handle;
static int speed = Rate;

/* maybe this should return -1 instead of sysfatal */
void
audiodevopen(void)
{
	if(snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0)
		error("snd_pcm_open failed");

	if(snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, speed, 1, 500000) < 0)
		error("snd_pcm_set_params");
}

void
audiodevclose(void)
{
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
}

void
audiodevsetvol(int what, int left, int right)
{
	if(what == Vspeed){
		speed = left;
		return;
	}
}

void
audiodevgetvol(int what, int *left, int *right)
{
	if(what == Vspeed){
		*left = *right = speed;
		return;
	}

	*left = *right = 100;
}

int
audiodevwrite(void *v, int n)
{
	snd_pcm_sframes_t frames;
	int tot, m;

	for(tot = 0; tot < n; tot += m){
		do {
			frames = snd_pcm_writei(handle, v+tot, (n-tot)/4);
		} while(frames == -EAGAIN);
		if (frames < 0)
			frames = snd_pcm_recover(handle, frames, 0);
		if (frames < 0)
			error("snd_pcm_writei");
		m = frames*4;
	}

	return tot;
}

int
audiodevread(void *v, int n)
{
	error("no reading");
	return -1;
}
