#ifndef _ROUTES_SERVICE_H_
#define _ROUTES_SERVICE_H_

struct _HttpReceive;
struct _HttpRequest;

// GET /api/director
extern void director_main_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/director/version
extern void director_version_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET *
extern void director_catch_all_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

#endif