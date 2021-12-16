## TWITCH-CLI ##

twitch-cli is a command line tool to watch twitch with mpv. It is made to be very simple and only serve as a way to connect mpv to twitch-cli faster than to type, as well as to give update on what streamer is live at any point.

Installation
------------

twitch-cli has two dependencies, mpv to start playing the stream and curl to find wether or not a streamer is live.

```
git clone https://github.com/TheBazz/twitch-cli
cd clone/build
cmake .. && make
sudo cp binary /usr/bin/twitch-cli
mkdir ~/.config/twitch-cli -p
touch ~/.config/twitch-cli/streamers.txt
```

I have plans to add twitch-tv to the aur, so very soon you'll be able to get it from there for arch users.


Usage
-----

First, go to ~/.config/twitch-cli/streamers.txt. In the file you should add the exact name of one streamer per line. To get the exact name, just go to twitch and search the streamer's name, the exact name is the name in the url. Once you have added streamers, you just need to type twitch-cli in terminal, and a list of the streamers you added will appear, as well as if they are live or not. You then enter the number of the streamer you want to watch and the stream should start with mpv.

Questions
---------

- I followed the installation, but it doesn't work.

The error you get in the terminal should come from mpv, so use it to fix mpv. A common error would be to not have configured yt-dlp as it is the current method to watch streams with mpv.

- I can't see twitch chat nor can i type.

This tool does not cover those features, I have heard that mpv has plugin to see twitch chat, but I don't think you can chat in it.

- What will be added in the future

For now, the project is done in c++, but I have decided to re-write it in c as a learning tool. So the entire project I guess if you're seeing this early. 
After that, I would like to make curl faster, I have seen different ways of doing that, from staying connected to the site so the requests are faster, to multithreading curl requests. If I can figure out how to add those, I'll add them.
