#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <time.h>
#include <signal.h>

#include <cerver/cerver.h>
#include <cerver/version.h>

#include <cerver/http/http.h>
#include <cerver/http/route.h>

#include <cerver/utils/log.h>
#include <cerver/utils/utils.h>

#include "service.h"
#include "version.h"

#include "routes/service.h"

static Cerver *director_service = NULL;

void end (int dummy) {

	if (director_service) {
		cerver_stats_print (director_service, false, false);
		(void) printf ("\nHTTP Cerver stats:\n");
		http_cerver_all_stats_print ((HttpCerver *) director_service->cerver_data);
		(void) printf ("\n");
		cerver_teardown (director_service);
	}

	service_end ();

	cerver_end ();

	#ifdef SERVICE_DEBUG
	(void) printf ("\n\nDone!\n\n");
	#endif

	exit (0);

}

static void director_set_service_routes (HttpCerver *http_cerver) {

	/*** top level route ***/

	// GET /api/director
	HttpRoute *main_route = http_route_create (REQUEST_METHOD_GET, "api/", director_main_handler);
	http_cerver_route_register (http_cerver, main_route);

	// HEAD /api/director
	http_route_set_handler (main_route, REQUEST_METHOD_HEAD, director_main_handler);

	/*** service routes ***/

	// GET /api/director/version
	HttpRoute *version_route = http_route_create (REQUEST_METHOD_GET, "version", director_version_handler);
	http_route_child_add (main_route, version_route);

	// HEAD /api/director/version
	http_route_set_handler (version_route, REQUEST_METHOD_HEAD, director_version_handler);

}

static void start (void) {

	director_service = cerver_create (
		CERVER_TYPE_WEB,
		"director-service",
		(u16) PORT,
		PROTOCOL_TCP,
		false,
		CERVER_CONNECTION_QUEUE
	);

	if (director_service) {
		/*** cerver configuration ***/
		cerver_set_alias (director_service, "director");

		cerver_set_receive_buffer_size (director_service, CERVER_RECEIVE_BUFFER_SIZE);
		cerver_set_thpool_n_threads (director_service, CERVER_TH_THREADS);
		cerver_set_handler_type (director_service, CERVER_HANDLER_TYPE_THREADS);

		cerver_set_reusable_address_flags (director_service, true);

		/*** web cerver configuration ***/
		HttpCerver *http_cerver = (HttpCerver *) director_service->cerver_data;

		director_set_service_routes (http_cerver);

		// add a catch all route
		http_cerver_set_catch_all_route (http_cerver, director_catch_all_handler);

		// admin configuration
		http_cerver_enable_admin_routes (http_cerver, true);

		if (cerver_start (director_service)) {
			cerver_log_error (
				"Failed to start %s!",
				director_service->info->name
			);

			cerver_delete (director_service);
		}
	}

	else {
		cerver_log_error ("Failed to create cerver!");

		cerver_delete (director_service);
	}

}

int main (int argc, char const **argv) {

	srand ((unsigned int) time (NULL));

	// register to the quit signal
	(void) signal (SIGINT, end);
	(void) signal (SIGTERM, end);

	// to prevent SIGPIPE when writting to socket
	(void) signal (SIGPIPE, SIG_IGN);

	cerver_init ();

	cerver_version_print_full ();

	service_version_print_full ();

	if (!service_init ()) {
		start ();
	}

	else {
		cerver_log_error ("Failed to init director!");
	}

	service_end ();

	cerver_end ();

	return 0;

}
