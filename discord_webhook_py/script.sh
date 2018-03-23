#
# Create a new webhook in your channel, then...
# Replace <ID> and <TOKEN> with a webhook token.

curl -X POST --data '{ "embeds": [{"title":"Test title", "description": "test text", "type": "rich", "color":"65535"}] }' -H "Content-Type: application/json" https://discordapp.com/api/webhooks/<ID>/<TOKEN>
