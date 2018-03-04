
1. Set up a project here:
	https://console.developers.google.com/cloud-resource-manager
	 * Click: Enable APIs and get credentials like keys
	 * Use youtube API v3
	 * Click "Create credentials"
	 * Select will be calling it from "Other UI"
	 * Select Public data.
	 * Get an API Key, and restrict by your IP address.
	My restricted API key for this project is: 
		AIzaSyA1XpoUMNDFOx0W4-HjiUI1uiahdfe20lE


2. Visit your project here:
	https://console.developers.google.com/home/dashboard?project=youtubecomments2

3.  https://developers.google.com/youtube/v3/live/docs/liveBroadcasts/list
      part = snippet / broadcstStatus = all / broadcstType = all

	Use OAUTH on this site to get the liveChatId
		EiEKGFVDRzd5SVd0VndjRU5nX1pTLW5haGc1ZxIFL2xpdmU


	Then you can use API Key here..

4. https://developers.google.com/youtube/v3/live/docs/liveChatMessages/list

	curl "https://www.googleapis.com/youtube/v3/liveChat/messages?liveChatId=EiEKGFVDRzd5SVd0VndjRU5nX1pTLW5haGc1ZxIFL2xpdmU&part=snippet&key=AIzaSyA1XpoUMNDFOx0W4-HjiUI1uiahdfe20lE"

	Totes works.

```		{
		 "kind": "youtube#liveChatMessageListResponse",
		 "etag": "\"_gJQceDMxJ8gP-8T2HLXUoURK8c/EktItOeGDW-qqheMbsHubaBnrS8\"",
		 "nextPageToken": "GMq3wfej0dkCIJOw_uCl0dkC",
		 "pollingIntervalMillis": 1526,
		 "pageInfo": {
		  "totalResults": 19,
		  "resultsPerPage": 19
		 },
		 "items": [
		  {
		   "kind": "youtube#liveChatMessage",
		   "etag": "\"_gJQceDMxJ8gP-8T2HLXUoURK8c/zsQSWiW4xjZfucBiPw3dxnPgr4c\"",
		   "id": "LCC.CiMSIQoYVUNHN3lJV3RWd2NFTmdfWlMtbmFoZzVnEgUvbGl2ZRI5ChpDT2ktXzhhZDBka0NGUkhVd1FvZENvRUt5dxIbQ0szM3VKNmMwZGtDRlFYQllBb2RKMWNNaFEw",
		   "snippet": {
			"type": "textMessageEvent",
			"liveChatId": "EiEKGFVDRzd5SVd0VndjRU5nX1pTLW5haGc1ZxIFL2xpdmU",
			"authorChannelId": "UCG7yIWtVwcENg_ZS-nahg5g",
			"publishedAt": "2018-03-03T22:45:18.656Z",
			"hasDisplayContent": true,
			"displayMessage": "test",
			"textMessageDetails": {
			 "messageText": "test"
			}
		   }
		  },```


	Then, to get the author name: 
		curl "https://www.googleapis.com/youtube/v3/channels?part=id,snippet,statistics,contentDetails,topicDetails&id=UCG7yIWtVwcENg_ZS-nahg5g&key=AIzaSyA1XpoUMNDFOx0W4-HjiUI1uiahdfe20lE"

		curl "https://www.googleapis.com/youtube/v3/channels?part=id,snippet,statistics,contentDetails,topicDetails&id=UCCtuJQDZFMZJUdPX6Qxxc8g&key=AIzaSyA1XpoUMNDFOx0W4-HjiUI1uiahdfe20lE"




