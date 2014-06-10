#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "just_string.h"
#include <jansson.h>

#define API "http://ajax.googleapis.com/ajax/services/search/web?v=1.0&q=%s"

/*
 * A few defines for formatting terminal output
 * I intend to replace these with some curses stuff later or something
 */
#define t_under		"\033[4m" 	/* underlined text */
#define t_bold		"\033[1m" 	/* bold text */
#define t_clear		"\033[0m"	/* removes previous colors/properties */

static size_t WriteMemoryCallback(void *contents, size_t size,
				  size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	string str = (string)userp;

	append_len_string(str, contents, realsize);

	return realsize;
}

int fetch_search(const char *query, string results)
{
	CURL *curl;
	CURLcode res;
	char *url = NULL;
	char *esc_query;
	size_t url_len;
	int n;
	int rc = -1;

	curl = curl_easy_init();
	if(curl == NULL) {
		fprintf(stderr, "Error initializing curl\n");
		return -1;
	}

	esc_query = curl_easy_escape(curl, query, 0);
	if(esc_query == NULL) {
		fprintf(stderr, "Error escaping query\n");
		goto exit;
	}

	/* few extra bytes can't hurt.. */
	url_len = strlen(esc_query) + strlen(API) + 1;
	url = malloc(url_len);

	n = snprintf(url, url_len, API, esc_query);
	if(n < 0 || n > url_len) {
		fprintf(stderr, "Error in API query format string.\n");
		goto exit;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)results);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	res = curl_easy_perform(curl);

	if(res != CURLE_OK) {
		fprintf(stderr, "Error communicating with API: %s\n",
			curl_easy_strerror(res)
		);
		goto exit;
	}

	rc = 0;

exit:
	if(esc_query)
		curl_free(esc_query);
	if(curl)
		curl_easy_cleanup(curl);
	curl_global_cleanup();
	if(url)
		free(url);

	return rc;
}

void print_help()
{
	fprintf(stderr, "Google Search Tool:\n");
	fprintf(stderr, "Usage:   google <search query>\n");
	fprintf(stderr, "Example: google github\n");
	fprintf(stderr, "Example: google \"blah blah blah\"\n");
	fprintf(stderr, "Example: google blah\\ blah\\ blah\n");
}

int main(int argc, char **argv)
{
	if(argc != 2) {
		print_help();
		return EXIT_FAILURE;
	}

	int n, i;
	int rc = EXIT_FAILURE;
	string results;
	json_t *j_root = NULL;
	json_t *j_responseStatus, *j_responseData, *j_results;
	json_t *j_result, *j_title, *j_content, *j_unescapedUrl;
	json_error_t error;

	results = new_string();
	if(results == NULL) {
		fprintf(stderr, "Out of memory\n");
		return EXIT_FAILURE;
	}

	n = fetch_search(argv[1], results);
	if(n < 0) {
		goto exit;
	}

	//printf("%s\n\n\n", get_string(results));

	/*
	 * C stands for Cowboy so lets round up those json results!
	 */

	j_root = json_loads(get_string(results), 0, &error);
	if(!j_root) {
		fprintf(stderr, "Error in json on line %d: %s\n", error.line, error.text);
		goto exit;
	}

	if(!json_is_object(j_root)) {
		fprintf(stderr, "Error: root is not an array\n");
		goto exit;
	}

	j_responseStatus = json_object_get(j_root, "responseStatus");
	if(!json_is_number(j_responseStatus)) {
		fprintf(stderr, "Error: responseStatus is not a number\n");
		goto exit;
	}
	if(json_integer_value(j_responseStatus) != 200) {
		fprintf(stderr, "Error: responseStatus is not 200\n");
		goto exit;
	}

	j_responseData = json_object_get(j_root, "responseData");
	if(!json_is_object(j_responseData)) {
		fprintf(stderr, "Error: responseData is not an object\n");
		goto exit;
	}

	j_results = json_object_get(j_responseData, "results");
	if(!json_is_array(j_results)) {
		fprintf(stderr, "Error: results is not an array\n");
		goto exit;
	}

	for(i = 0; i < json_array_size(j_results); i++) {
		j_result = json_array_get(j_results, i);
		if(!json_is_object(j_result)) {
			fprintf(stderr, "Error: result %d is not an object\n",
				i + 1);
			goto exit;
		}
		j_content = json_object_get(j_result, "content");
		if(!json_is_string(j_content)) {
			fprintf(stderr, "Error: result %d content is not a string\n",
				i + 1);
			goto exit;
		}
		j_title = json_object_get(j_result, "title");
		if(!json_is_string(j_title)) {
			fprintf(stderr, "Error: result %d title is not an string\n",
				i + 1);
			goto exit;
		}
		j_unescapedUrl = json_object_get(j_result, "unescapedUrl");
		if(!json_is_string(j_unescapedUrl)) {
			fprintf(stderr, "Error: result %d unescapedUrl is not an string\n",
				i + 1);
			goto exit;
		}

		printf(t_bold "Result:" t_clear " %d: " t_bold "Url: " t_clear
			t_under "%s" t_clear t_bold " Title:" t_clear " %s " t_bold "Content:"
			t_clear " %s\n\n",
		       i, json_string_value(j_unescapedUrl), json_string_value(j_title),
		       json_string_value(j_content));
	}

	rc = EXIT_SUCCESS;

exit:
	if(j_root)
		json_decref(j_root);
	free_string(results);
	return rc;
}










