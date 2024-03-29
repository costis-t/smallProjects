# Turn on speakers only when system needs them
It's a pity that the recent electricity bill hikes urged me to become more environmentally friendly than I already was, but better late than never.

I decided to explore how I can reduce my power consumption apart from the obvious ways.
To my surprise, I discovered that my speakers consume around ~42 Watt per hour. 
That's a lot!

I have them plugged in as a slave device in a Master-Slave power strip with my workstation as the Master, so they are only turned on when my system is turned on.
Nevertheless, I only use them for a few hours per day, when I take a call or listen to music while I work, say for 4 hours per day.
With the old electricity rates, I was paying around 0.25 Euro per KWh, while with the new around 0.75.

Assuming that I have my system turned on for about 14 hours per day, I was previously paying around $\frac{42 \cdot 14 \cdot 365}{1000} \cdot 0.25 \simeq 53.67$ Euro per year.
Now, I will have  to pay $\frac{42 \cdot 14 \cdot 365}{1000} \cdot 0.75 \simeq 160.97$ Euro per year.
This is unacceptable for just a few hours of daily use.


I decided to create a [systemd timer][1] to monitor whenever my applications produce audio.
Then, I use a [smart socket](https://solution.tuya.com/projects/CMa4p001lsjhns) with a custom file sensor for [Home Assistant][2] to turn the power on and off depending on whether the applications produce sound.

The bash script below shows an example for how one can filter the output of 
`pacmd list-sink-inputs` with [AWK](https://en.wikipedia.org/wiki/AWK), [sed](https://en.wikipedia.org/wiki/Sed), 
[grep](https://en.wikipedia.org/wiki/Grep), [regular expressions](https://en.wikipedia.org/wiki/Regular_expression) and 
[pipelines](https://en.wikipedia.org/wiki/Pipeline_(Unix)), when the Google Chrome browser produces audio:
```bash
#!/bin/bash

f_sink_output() {
    pacmd list-sink-inputs |
        grep -e "flags\|state\|muted\|application.name " |
        sed "s/^\([^=]*\) = /\1: /;s/^[ \t]*/ /" |
        tr "\n" " " |
        awk -v OFS=' ' '{$1=$1}1'
}

v_sink_output="$(f_sink_output)"

if [[ $v_sink_output == *"muted: no"* ]] && [[ $v_sink_output == *"Chrome"* ]]; then
    echo '1' > /tmp/sound_playing
else
    echo '0' > /tmp/sound_playing
fi
```
The output of `pacmd list-sink-inputs` when the speakers are being used by the system is this one:
```bash
1 sink input(s) available.
    index: 65
        driver: <protocol-native.c>
        flags: START_CORKED 
        state: RUNNING
        sink: 2 <alsa_output.pci-0000_28_00.3.analog-stereo>
        volume: front-left: 99957 / 153% / 11.00 dB,   front-right: 99957 / 153% / 11.00 dB
                balance 0.00
        muted: no
        current latency: 34.04 ms
        requested latency: 26.00 ms
        sample spec: float32le 2ch 44100Hz
        channel map: front-left,front-right
                     Stereo
        resample method: copy
        module: 12
        client: 10640 <Google Chrome>
        properties:
                application.icon_name = "google-chrome"
                media.name = "Playback"
                application.name = "Google Chrome"
                native-protocol.peer = "UNIX socket client"
                native-protocol.version = "censored"
                application.process.id = "censored"
                application.process.user = "censored"
                application.process.host = "censored"
                application.process.binary = "chrome"
                application.language = "censored"
                window.x11.display = ":0"
                application.process.machine_id = "censored"
                application.process.session_id = "2"
                module-stream-restore.id = "sink-input-by-application-name:Google Chrome"
```
and the `f_sink_output` function of the above script merely transforms it to this string:

```
flags: START_CORKED state: RUNNING muted: no application.name: "Google Chrome"
```
You can watch a demonstration on [YouTube](https://www.youtube.com/watch?v=qeeKuwXsRtk).

I activated the timer on 30 August, and I can already see a nice consumption reduction with the help of [Enelogic](https://enelogic.com/) which tracks smart meters for free.
![image search api](https://github.com/costis-t/smallProjects/blob/master/energy_savings/images/Screenshot_20220904_004345.png)



  [1]: https://wiki.gentoo.org/wiki/Systemd#Timer_services
  [2]: https://www.home-assistant.io/
