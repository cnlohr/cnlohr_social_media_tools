#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cnsslclient.h>
#include <cnhttpclient.h>
#include <jsmn.h>
#include <unistd.h>

#ifndef LOADFILEDEFINED
#define LOADFILEDEFINED
static int LoadFileLineIntoBuffer( const char * folder_prefix, const char * file, char * buffer, int buffersize )
{
	char filename[1024];
	snprintf( filename, 1024, "%s/%s", folder_prefix, file );
	FILE * f = fopen( filename, "r" );
	if( !f )
	{
		fprintf( stderr, "Error: can't get %s\n", file );
		return -1;
	}
	int c;
	int i = 0;
	while( ( c = fgetc( f ) ) != EOF && i < buffersize-1 )
	{
		if( c == '\n' ) break;
		buffer[i++] = c;
	}
	buffer[i] = 0;
	fclose( f );
	return i;
}
#endif

static int LoadFileLineByNumberIntoBuffer(const char* folder_prefix, const char* file, int line, char* buffer, int buffersize)
{

	char filename[1024];
	snprintf(filename, 1024, "%s/%s", folder_prefix, file);
	FILE* f = fopen(filename, "r");
	if (!f)
	{
		fprintf(stderr, "Error: can't get %s\n", file);
		return -1;
	}
	int c;
	int i = 0;
	while ((c = fgetc(f)) != EOF && i < buffersize - 1)
	{
		if (c == '\n')
		{
			
			if (line == 0)
			{
				break;
			}
			line--;
		}
		else if (line == 0) 
		{
			buffer[i++] = c;
		}
	}
	buffer[i] = 0;
	fclose(f);
	return i;
}


const char * GetTokenByName( char * origtext, jsmntok_t * tok )
{
	origtext[tok->end] = 0;
	return origtext + tok->start;
}

void ProcessNextElement( jsmntok_t ** tok )
{
	int nr_children = (*tok)->size;
	int j;
	//printf( "Process Next: %d\n", nr_children );
	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		ProcessNextElement( tok );
	}
}



const char* ReadInsideChatSnippet(char* origtext, jsmntok_t** tok)
{
	const char* msgo = 0;
	int j;
	int nr_children = (*tok)->size;

	for (j = 0; j < nr_children; j++)
	{
		(*tok)++;
		const char* st = GetTokenByName(origtext, *tok);

		if (strcmp(st, "displayMessage") == 0)
		{
			msgo = GetTokenByName(origtext, (*tok) + 1);
			ProcessNextElement(tok);
		}
		else
		{
			ProcessNextElement(tok);
		}
	}
	return msgo;
}


const char * ReadInsideChatAuthorDetails( char * origtext, jsmntok_t ** tok )
{
	const char * auth = 0;
	int j;
	int nr_children = (*tok)->size;

	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		const char * st = GetTokenByName( origtext, *tok );

		if( strcmp( st, "displayName" ) == 0 )
		{
			auth = GetTokenByName( origtext, (*tok) + 1 );
			ProcessNextElement( tok );
		}

		ProcessNextElement( tok );
	}
	return auth;
}


void ReadChatEntry( char * origtext, jsmntok_t ** tok, const char ** chatsnip, const char ** authorsnip )
{
	//Found items... Advnce to next token.
	int j;
	int nr_children = (*tok)->size;

	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		const char * st = GetTokenByName( origtext, *tok );
		
		if( strcmp( st, "snippet" ) == 0 )
		{
			(*tok)++;
			*chatsnip = ReadInsideChatSnippet( origtext, tok );
		}
		else if( strcmp( st, "authorDetails" ) == 0 )
		{
			(*tok)++;
			*authorsnip = ReadInsideChatAuthorDetails( origtext, tok );
		}
		else
		{
			ProcessNextElement( tok );
		}
	}
}

char* ProcessGetLiveChatResponseV1API(char* origtext, jsmntok_t* tokens, int tottoks, char** continuation) {
	int retlen = 0;
	int retmalloc = 1024;
	char* ret = malloc(retmalloc);

	int to;
	char messageAuthor[64];
	char messageText[256];
	for (to = 1; to < tottoks; to ++)
	{
		if (tokens[to].type == JSMN_STRING)
		{
			
			char* start = &(origtext)[tokens[to].start];
			int size = tokens[to].end - tokens[to].start;
			char key[size + 1];
			memcpy(key, start, size);
			key[size] = '\0';
			int comparison = strcmp(key, "liveChatContinuation");
			//printf("%s-%d\n", key, comparison);
			if (comparison == 0)
			{
				int offset = 0;
				
				char value[512];
				do
				{
					offset++;
					memset(value, 0, sizeof(value) * sizeof(char));
					start = &(origtext)[tokens[to + offset].start];
					size = tokens[to + offset].end - tokens[to + offset].start;
					if (size > 512) { continue;}
					memcpy(value, start,size);
					value[size] = '\0';
					//printf("\nvalue: %s\n", value);
					
					
				} while (strcmp(value, "continuation")!=0);
				offset++;
				memset(value, 0, sizeof(value) * sizeof(char));
				
				start = &(origtext)[tokens[to + offset].start];
				size = tokens[to + offset].end - tokens[to + offset].start;
				memcpy(value, start, size);
				value[size] = '\0';

				//printf("\nvalue: %s\n", value);
				
				//*continuation = value;
				memset(*continuation, 0, sizeof(*continuation) * sizeof(char));
				//printf("erased");
				memcpy(*continuation, value, sizeof(value));
				//printf("CONT: '%s'\n", value);
				//printf("CONTINUATION: '%s'\n", *continuation);

				


			}else if (strcmp(key, "liveChatTextMessageRenderer") == 0)
			{
				

				int of=2;
				int authorOffset=0;
				int textOffset=0;
				
				while (of < 100 && (!authorOffset || !textOffset))
				{
					//printf("testing %d\n", of);
					char* start = &(origtext)[tokens[to + of].start];
					int size = tokens[to + of].end - tokens[to + of].start;
					char key[size + 1];

					memcpy(key, start, size);
					key[size] = '\0';

					//printf("testing %d\n", key);
					if (strcmp(key, "simpleText")==0)
					{
						authorOffset = of + 1;
						//printf("author: %d\n", authorOffset);
						
					}else if (strcmp(key, "text")==0)
					{
						textOffset = of + 1;
						//printf("text: %d\n", textOffset);
					}
					of++;
				}
				if (authorOffset && textOffset)
				{
					char* start = &(origtext)[tokens[to + authorOffset].start];
					int size = tokens[to + authorOffset].end - tokens[to + authorOffset].start;
					char author[size + 1];
					memcpy(author, start, size);
					author[size] = '\0';
					memcpy(messageAuthor, author, sizeof(author));

					start = &(origtext)[tokens[to + textOffset].start];
					size = tokens[to + textOffset].end - tokens[to + textOffset].start;
					char text[size + 1];
					memcpy(text, start, size);
					text[size] = '\0';
					memcpy(messageText, text, sizeof(text));


				
					if ((retlen + 1024) > retmalloc)
					{
						retmalloc += 1024;
						ret = realloc(ret, retmalloc);
					}
					//printf("%s\t%s\n", messageAuthor, messageText);
					retlen += sprintf(ret + retlen, "%s\t%s\n", messageAuthor, messageText);
										
				}
				memset(messageAuthor, 0, 64 * sizeof(char));
									
				memset(messageText, 0, 256 * sizeof(char));

			}

		}

	}
	ret[retlen] = 0;
	
	return ret;

}

/*
if( (retlen + 1024) > retmalloc )
					{
						retmalloc += 1024;
						ret = realloc( ret, retmalloc );
					}

					retlen += sprintf( ret + retlen, "%s\t%s\n", authorsnip, chatsnip );
				}
			}
		}
		ProcessNextElement( tok );
	}
	ret[retlen] = 0;
	return ret;
	*/

char * ProcessChatMessageResponse(char * origtext, jsmntok_t ** tok, jsmntok_t * tokend, int * pollinfo, char ** NextPageToken, int show_history )
{
	int retlen = 0;
	int retmalloc = 1024;
	char * ret = malloc( retmalloc );

	int nr_children = (*tok)->size;
	int j;

	for( j = 0; j < nr_children; j++ )
	{
		(*tok)++;
		const char * st = GetTokenByName( origtext, *tok );

		if( strcmp( st, "pollingIntervalMillis" ) == 0 )
		{			
			const char * st = GetTokenByName( origtext, *tok + 1 );
			if( pollinfo ) *pollinfo = atoi( st );
		}
		else if( strcmp( st, "nextPageToken" ) == 0 )
		{
			if( NextPageToken )
			{
				if( *NextPageToken ) free( *NextPageToken );
				*NextPageToken = strdup( GetTokenByName( origtext, *tok + 1 ) );
			}
		}
		else if( strcmp( st, "items" ) == 0 )
		{
			(*tok)++;
			int nr_children = (*tok)->size;
			int i;
			for( i = 0; i < nr_children; i++ )
			{
				(*tok)++;
				const char * chatsnip, *authorsnip;
				chatsnip = authorsnip = 0;
				ReadChatEntry( origtext, tok, &chatsnip, &authorsnip );

				if( show_history )
				{
					if( (retlen + 1024) > retmalloc )
					{
						retmalloc += 1024;
						ret = realloc( ret, retmalloc );
					}

					retlen += sprintf( ret + retlen, "%s\t%s\n", authorsnip, chatsnip );
				}
			}
		}
		ProcessNextElement( tok );
	}
	ret[retlen] = 0;
	return ret;
}

char* GetLivechatDataV1API(const char* apikey, char** continuation)
{
	char curlurlbase[8192];
	char curlurl[8192];
	char apiKeybuff[128];
	char continuationbuff[512];
	const char* reqtype = "authorDetails,snippet";

	if (apikey == 0 || apikey[0] == '-')
	{
		int len = LoadFileLineIntoBuffer("..", "ytInitialData.txt", apiKeybuff, sizeof(apiKeybuff));
		if (len < 5)
		{
			fprintf(stderr, "Error: apiKey in ytInitialData Invalid, please, run ytstreamstats\n");
			return strdup("");
		}
		apikey = apiKeybuff;
	}
	//printf("%s\n", apikey);
	
	
	//printf("Continuation: %s\n", *continuation);
	if (*continuation == 0 || *continuation[0] == '-')
	{
		int len = LoadFileLineByNumberIntoBuffer("..", "ytInitialData.txt", 1, continuationbuff, sizeof(continuationbuff));
		//printf(continuationbuff);
		if (len < 5)
		{
			fprintf(stderr, "Error: Continuation Invalid, please, run ytstreamstats\n");
			
			return strdup("");
		}
		*continuation = continuationbuff;
	}

	//printf("CONTINUATION TOKEN: %s\n", *continuation);

	int Oauth = 0;
	if (Oauth)
	{

		sprintf(curlurlbase, "https://www.youtube.com/youtubei/v1/live_chat/get_live_chat?commandMetadata=%%5Bobject%%20Object%%5D&continuation=%s&pbj=1", *continuation);
		
	}
	else {

		sprintf(curlurlbase, "https://www.youtube.com/live_chat/get_live_chat?commandMetadata=%%5Bobject%%20Object%%5D&continuation=%s&pbj=1", *continuation);
			

	}

	//printf("%s\n",curlurlbase);

	struct cnhttpclientrequest req;
	memset(&req, 0, sizeof(req));
	req.host = 0;
	req.port = 0;
	req.URL = curlurl;
	req.AuxData = 0;
	req.AuxDataLength = 0;

	sprintf(curlurl, "%s",
		curlurlbase);

	char auxhead[8192];
	sprintf(auxhead, "user-agent: %s", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4147.105 Safari/537.36");
	req.AddedHeaders = auxhead;


	//Get a bunch of messages
	struct cnhttpclientresponse* r = CNHTTPClientTransact(&req);

	//r->payload[r->payloadlen - 1] = 0;
	//printf("%s\n", curlurl);
	//printf("%d\n", r->payloadlen);
	
	//Uncomment this if you aren't getting anything.
	FILE* f = fopen("../payload.html", "wb");
	fprintf( f,"%s\n", r->payload );
	fclose(f);

	jsmn_parser jsmnp;

	
	jsmn_init(&jsmnp);
	int tokenTotal = jsmn_parse(&jsmnp, r->payload, r->payloadlen,
		NULL, 1024);

	//printf("Number of expected TOKENS: %d\n", tokenTotal);
	//printf("%s", r->payload);
	jsmntok_t * tokens=malloc(sizeof(jsmntok_t)*tokenTotal);
	memset(tokens,0, sizeof(jsmntok_t) * tokenTotal);

	jsmn_init(&jsmnp);
	int tottoks = jsmn_parse(&jsmnp, r->payload, r->payloadlen,
		tokens, tokenTotal);

	//printf("Total TOKENS: %d\n", tottoks);

	

	char* ret = 0;

	if (tottoks > 0)
	{
		ret = ProcessGetLiveChatResponseV1API(r->payload, tokens, tottoks, continuation);
		
	}
	else
	{
		fprintf(stderr, "Error %p parsing the JSON : %s\n", &tokens, r->payload);
		ret = 0;
	}
	free(tokens);
	CNHTTPClientCleanup(r);
	return ret;

}

char * GetLivechatData( const char * livechatid, char ** nextpagetoken, int include_history, int * pollinfo )
{
	char curlurlbase[8192];
	char curlurl[16384];
	char livechatbuff[128];
	const char * reqtype = "authorDetails,snippet";

	if( livechatid == 0 || livechatid[0] == '-' )
	{
		int len = LoadFileLineIntoBuffer( "..", "live_chat_id.txt", livechatbuff, sizeof( livechatbuff ) );
		if( len < 5 )
		{
			fprintf( stderr, "Error: Live chat ID Invalid\n" );
			return strdup("");
		}
		livechatid = livechatbuff;
	}
	
	sprintf( curlurlbase, "https://www.googleapis.com/youtube/v3/liveChat/messages?liveChatId=%s&",livechatid);

	char apikey[8192];
	int uses_api_key = 0;

	{
		FILE * f = fopen( "../.ytapikey.txt", "r" );
		if(f)
		{
			fscanf( f, "%8100s", apikey );
			fclose( f );	
			uses_api_key = 1;
		}
		sprintf( curlurlbase + strlen(curlurlbase), "key=%s&",apikey);
	}

	struct cnhttpclientrequest req;
	memset( &req, 0, sizeof( req ) );
	req.host = 0;
	req.port = 0;
	req.URL = curlurl;
	req.AuxData = 0;
	req.AuxDataLength = 0;


	if( !uses_api_key )
	{
		char oauthbear[4096];
		FILE * f = fopen( "../.oauthtoken.txt", "r" );
		if( !f )
		{
			fprintf( stderr, "Error: no oauth token found.  Run yt_oauth_helper\n" );
			return 0;
		}
		fscanf( f, "%4095s", oauthbear );
		fclose( f );	
		char auxhead[8100];
		sprintf( auxhead, "Authorization: Bearer %s", oauthbear );
		req.AddedHeaders = auxhead;
	}

	snprintf( curlurl, sizeof(curlurl)-1, "%spart=%s%s%s",
		curlurlbase,
		reqtype,
		(nextpagetoken&&*nextpagetoken)?"&pageToken=":"",
		(nextpagetoken&&*nextpagetoken)?*nextpagetoken:"" );

	//printf("%s\n", curlurl );

	//Get a bunch of messages
	struct cnhttpclientresponse * r = CNHTTPClientTransact( &req );

	r->payload[r->payloadlen-1] = 0;
	//Uncomment this if you aren't getting anything.
	//printf( "PAYLOAD: %s\n", r->payload );

	int tokenlen = 131072;
	jsmntok_t * tokens = malloc( sizeof( jsmntok_t ) * tokenlen );
	memset( tokens, 0, sizeof( jsmntok_t ) * tokenlen  );
	jsmn_parser jsmnp;


	jsmn_init( &jsmnp );
	int tottoks = jsmn_parse(&jsmnp, r->payload, r->payloadlen,
		tokens, tokenlen );

	char * ret = 0;

	if( tottoks > 0 )
	{
		jsmntok_t * tok = &tokens[0];
		ret = ProcessChatMessageResponse( r->payload, &tok, &tokens[tottoks], pollinfo, nextpagetoken, include_history );
	}
	else
	{
		fprintf( stderr, "Error %d parsing the JSON : %s\n", tottoks, r->payload );
		ret = 0;
	}
	free( tokens );
	CNHTTPClientCleanup( r );
	return ret;
}


#ifdef BUILD_EXE

int main( int argc, char ** argv )
{

	int show_history = 0;
	int notfirst = 0;
	int pollinfo = 0;
	if (argc == 4)
	{
		const char* apiKey= argv[1];
		char* continuation = argv[2];
		show_history = atoi(argv[3]);
		char* ret = GetLivechatDataV1API(apiKey, &continuation);

		
		if (ret)
		{
			printf("%s", ret);
			fflush(stdout);
			
		}

		while (1)
		{
			fflush(stdout);
			ret = GetLivechatDataV1API(apiKey, &continuation);


			if (ret)
			{
				printf("%s", ret);
				fflush(stdout);
			}
			else
				sleep(2);
		}

	}

	else if( argc == 3 )
	{

		show_history = atoi(argv[2]);
		const char* livechatid = argv[1];//"EiEKGFVDRzd5SVd0VndjRU5nX1pTLW5haGc1ZxIFL2xpdmU";
		char* nextpagetoken = 0;
		char* ret = GetLivechatData(livechatid, &nextpagetoken, show_history, &pollinfo);

		if (ret)
		{
			printf("%s", ret);
			fflush(stdout);
		}

		//Pollinfo is just too short.  We blow our quota :(
//		if( pollinfo )
//			usleep( pollinfo * 1000 );
//		else
//			sleep(2);
	}
	else 
	{
		fprintf(stderr, "Error! Usage:");
		fprintf(stderr, "                 ./chatmon [livechatid use - if you want to use id from ytstreamstats] [show_history (0/1)]\n");
		fprintf(stderr, "                 or");
		fprintf(stderr, "                 ./chatmon [apiKey|- if you want to use id from ytstreamstats] [continue|- if you want to use id from ytstreamstats] [show_history (0/1)]\n");
		fprintf(stderr, "Reads youtube livestream comments and sends to STDOUT. Format: [user]\\t[text]\\n\n");
		return -5;

	}


}
#endif

