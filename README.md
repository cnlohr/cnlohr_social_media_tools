# various social media tools

First, you will need your youtube oauth creds. 

Log into your API Dashboard.
 * https://console.developers.google.com/
 * Select your project and go to the youtube v3 api section.
 * Create new oauth credentials (or use existing)
 * Select type for new oauth app to be "other"
 * Get the CLIENT_ID and note the SECRET_KEY.
 * Write them into `.client_id.txt` and `.client_secret.txt`.

Next, get your oauth keys.
```
cd ytoauthhelper
./ytoauthhelper
```

Follow on-screen prompts.  It should write your oauth key into `.oauthtoken.txt`.

For example, you can do the following:
```cd ytstreamstats
./ytstreamstats [stream id]```

or
```
./runytchatmon.sh EiEKGFVDRzd5SVd0VndjRU5nX1pTLW5haGc1ZxIFL2xpdmU | tabformatter/tabformatter | tee chatlog.txt |  ./addtime.sh | ./rundiscordposter.sh
```

or
```
cd ytposter
./ytposter [stream id] chat message
```

