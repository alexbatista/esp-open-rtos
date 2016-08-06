/* http_get - Retrieves a web page over HTTP GET.
 *
 * See http_get_ssl for a TLS-enabled version.
 *
 * This sample code is in the public domain.,
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"

/* Go inside and change the PASSWORD and SSID for your connection. */
#include "ssid_config.h"

#include "FreeRTOS.h"
#include "task.h"

#include "mongoose.h"

#define MG_TASK_STACK_SIZE 4098
#define MG_TASK_PRIORITY 2

static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;
struct mg_mgr mgr;

static void handle_sum_call(struct mg_connection *nc, struct http_message *hm) {
  char n1[100], n2[100];
  double result;

  /* Get form variables */
  mg_get_http_var(&hm->query_string, "n1", n1, sizeof(n1));
  mg_get_http_var(&hm->query_string, "n2", n2, sizeof(n2));

  /* Send headers */
  mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");

  /* Compute the result and send it back as a JSON object */
  result = strtod(n1, NULL) + strtod(n2, NULL);
  mg_printf_http_chunk(nc, "{ \"result\": %lf }", result);
  mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (mg_vcmp(&hm->uri, "/api/v1/sum") == 0) {
        handle_sum_call(nc, hm); /* Handle RESTful call */
      } else if (mg_vcmp(&hm->uri, "/printcontent") == 0) {
        char buf[100] = {0};
        memcpy(buf, hm->body.p,
               sizeof(buf) - 1 < hm->body.len ? sizeof(buf) - 1 : hm->body.len);
        printf("%s\n", buf);
      } else {
        mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
      }
      break;
  }
}



static void mg_task(void *arg) {

  struct mg_connection *nc;
 // int i;
 // char *cp;

  mg_mgr_init(&mgr, NULL);

  /* Set HTTP server options */
  nc = mg_bind(&mgr, s_http_port, ev_handler);
  if (nc == NULL) {
    fprintf(stderr, "Error starting server on port %s\n", s_http_port);
    return;
  }

  mg_set_protocol_http_websocket(nc);

  printf("Starting RESTful server on port %s\n", s_http_port);

	while (1) {
        mg_mgr_poll(&mgr, 1000);
    }

  return;
}

void user_init(void)
{
    uart_set_baud(UART0, 115200);
    printf("Running mongoose http example\r\nSDK version: %s\r\n", sdk_system_get_sdk_version());
    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS
    };

    /* required to call wifi_set_opmode before station_set_config */
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    xTaskCreate(&mg_task, (signed char *)"mongoose", MG_TASK_STACK_SIZE, NULL, MG_TASK_PRIORITY, NULL);
}

