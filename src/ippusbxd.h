#pragma once

#include <libusb.h>
#include <pthread.h>
#include <stdint.h>

#include "tcp.h"
#include "usb.h"

struct service_thread_param {
  /* Connection to the device issuing requests to the printer. */
  struct tcp_conn_t *tcp;
  /* Socket which holds the context for the bound USB printer. */
  struct usb_sock_t *usb_sock;
  /* Represents a connection to a specific USB interface. */
  struct usb_conn_t *usb_conn;
  pthread_t thread_handle;
  uint32_t thread_num;
  pthread_cond_t *cond;
};

struct libusb_callback_data {
  /* Represents whether there is currently an active attempt to read from the
     printer. */
  int *read_inflight;
  /*
   * Indicates that the previous read response was empty. This is used to
   * perform exponential backoff in service_printer_connection() to avoid
   * overloading the printer with read requests when there is nothing to read.
   */
  int *empty_response;
  uint32_t thread_num;
  struct tcp_conn_t *tcp;
  /* The contents of the response from the printer. */
  struct http_packet_t *pkt;
  pthread_mutex_t *read_inflight_mutex;
  pthread_cond_t *read_inflight_cond;
};

/* Constants */

/* Times to wait in milliseconds before sending another read request to the
   printer. */
const int initial_backoff = 100;
const int maximum_backoff = 1000;

/* Function prototypes */

/* Handles connection requests and
   is run in a separate thread. It detaches itself from the main thread and sets
   up a USB connection with the printer. This function spawns a partner thread
   which is responsible for reading from the printer, and then this function
   calls into service_socket_connection() which is responsible for reading from
   the socket which made the connection request. Once the socket has closed its
   end of communiction, this function notifies its partner thread that the
   connection has been closed and then joins on the partner thread before
   shutting down. */
void *service_connection(void *params_void);

/* Reads from the connected socket in |params| and writes any
   received messages to the printer. */
void service_socket_connection(struct service_thread_param *params);

/* Reads from messages from the printer and writes any responses to the
   connected socket in |params_void|. */
void *service_printer_connection(void *params_void);

/* Attempts to allocate space for a tcp socket. If the allocation is
   successful then a value of 0 is returned, otherwise a non-zero value is
   returned. */
int allocate_socket_connection(struct service_thread_param *param);

/* Attempts to setup a connection for to a tcp socket. Returns a 0 value on
   success and a non-zero value if something went wrong attempting to establish
   the connection. */
int setup_socket_connection(struct service_thread_param *param);

/* Attempts to create a new usb_conn_t and assign it to |param| by acquiring an
   available usb interface. Returns 0 if the creation of the connection struct
   was successful, and non-zero if there was an error attempting to acquire the
   interface. */
int setup_usb_connection(struct usb_sock_t *usb_sock,
                         struct service_thread_param *param);

/* Attempts to register a new communication thread to execute the function
   |routine| with the given |params|. Returns 0 if successful, and a non-zero
   value otherwise. */
int setup_communication_thread(void *(*routine)(void *),
                               struct service_thread_param *param);

/* Creates a new libusb_callback_data struct with the given paramaters. */
struct libusb_callback_data *setup_libusb_callback_data(
    struct http_packet_t *pkt, int *read_inflight, int *empty_response,
    struct service_thread_param *thread_param,
    pthread_mutex_t *read_inflight_mutex);

/* Returns the value of |read_inflight|. The given |read_inflight_mutex| is used
   to lock changes to |read_inflight| as another thread performing the
   asynchronous transfer with the printer may change the value upon completion.
 */
int get_read_inflight(const int *read_inflight,
                      pthread_mutex_t *read_inflight_mutex);

/* Sets the value of |read_inflight| to |val|. Uses |mtx| as another thread
   which is processing the asynchronous transfer may change the value once the
   transfer is complete. */
void set_read_inflight(int val, pthread_mutex_t *mtx, int *read_inflight);

/* Returns a non-zero value if the communication socket in |param| is currently
   open for communication. */
int is_socket_open(const struct service_thread_param *param);

/* Accepts the given |backoff| value used to determine the time to wait between
   unsuccessful asynchronous printer transfers and returns the updated value.
   The updated value will not exceed |maximum_backoff|. */
int update_backoff(int backoff);
