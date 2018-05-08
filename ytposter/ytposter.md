Log into your API Dashboard.
https://console.developers.google.com/
Select your project and go to the youtube section.
Create new oauth credentials.

Select type for new oauth app to be "other"

Get the CLIENT_ID and note the SECRET_KEY.

Write them into ../client_id.txt and../client_secret.txt.

Run ./yt_oauth_helper and log in.

Voila you have access...

curl -H "Authorization: Bearer [code]" "https://www.googleapis.com/youtube/v3/channels?part=id&mine=true" 

And it works.


