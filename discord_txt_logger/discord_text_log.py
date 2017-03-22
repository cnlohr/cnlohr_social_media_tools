#!/usr/bin/python3
import discord
import asyncio

'''
Python script to make discord put out a channel log.
This will be a plain-text channel log for your archival and searching happiness!

Based on : https://github.com/Rapptz/discord.py

Install:

apt-get install python3-pip
pip3 install --upgrade pip
pip3 install setuptools
pip3 install discord.py

Set up a bot.
Go here: https://discordapp.com/developers/applications/me
Be sure to make it an app bot user.
Type in a name and description.
You will get a ID and a secret:

ID: 294186nnnnnnnnnn8
Secret:  97MIqXXXXXXxxxxxxxxxxxxxxxxxxxxx

Visit: https://discordapp.com/oauth2/authorize?&client_id=YOUR_CLIENT_ID_HERE&scope=bot&permissions=3072
Visit: https://discordapp.com/oauth2/authorize?&client_id=294186nnnnnnnnnn8
Replace "client.run("token");" 's TOKEN with your token. (private token)
'''

client = discord.Client()

@client.event
async def on_ready():
    for server in client.servers:
        for channel in server.channels:
            if channel.name == "libsurvive":
                print( channel.name )
                async for log in client.logs_from(channel, limit=10000):
                    lastMessage = log
                    print( "#" + str(log.timestamp) + "#" + log.author.name + "#" + log.content )
                exit()

# Below is your bot SECRET key.
client.run('---')
