#include <cassert>
#include <iostream>
#include <mpd/client.h>
#include <mpd/connection.h>
#include <mpd/error.h>
#include <mpd/player.h>
#include <mpd/song.h>
#include <mpd/status.h>
#include <mpd/tag.h>

static std::string APPNAME = "app";

static void play(struct mpd_connection *conn) {
  std::cout << "Restarting..." << std::endl;
  mpd_run_play(conn);
}

static void pause(struct mpd_connection *conn, bool currentState) {
  std::cout << "Toggling..." << std::endl;
  mpd_run_pause(conn, currentState);
}

static void skip(struct mpd_connection *conn) {
  std::cout << "Skipping..." << std::endl;
  mpd_run_next(conn);
}

static void displayHelpMenu() {
  std::cout << std::endl;
  std::cout << APPNAME << ": an mpd client written in C++" << std::endl;
  std::cout << "usage: " << APPNAME << " [options]" << std::endl;
  std::cout << "options:" << std::endl;
  std::cout << "         play:    play current song" << std::endl;
  std::cout << "         pause:   pause current song" << std::endl;
  std::cout << "         skip:    skip song" << std::endl;
  std::cout << " --help, help:   display this menu" << std::endl;
  std::cout << std::endl;
}

static void returnSong(struct mpd_connection *conn, struct mpd_status *status,
                       struct mpd_song *song) {

  std::cout << "Playing: " << mpd_song_get_tag(song, MPD_TAG_ARTIST, 0) << " \""
            << mpd_song_get_tag(song, MPD_TAG_ALBUM, 0) << "\" ("
            << mpd_song_get_tag(song, MPD_TAG_DATE, 0) << ") - "
            << mpd_song_get_tag(song, MPD_TAG_TITLE, 0) << std::endl;
}

// https://github.com/MusicPlayerDaemon/libmpdclient/blob/master/src/example.c
// dont mind me its useful...
static int handle_error(struct mpd_connection *c) {
  assert(mpd_connection_get_error(c) != MPD_ERROR_SUCCESS);

  // fprintf(stderr, "%s\n", mpd_connection_get_error_message(c));
  switch (mpd_connection_get_error(c)) {
  case MPD_ERROR_OOM:
    std::cerr << "Out of Memory" << std::endl;
    break;
  case MPD_ERROR_CLOSED:
    std::cerr << "Connection closed by MPD" << std::endl;
    break;
  case MPD_ERROR_TIMEOUT:
    std::cerr << "Timeout";
    break;
  case MPD_ERROR_SYSTEM:
    std::cerr << "System error";
    break;
  default:
    std::cerr << "Unknown error" << std::endl;
    break;
  }

  mpd_connection_free(c);
  return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
  // small mpd app to play and pause music
  if (argc < 2) {
    std::cout << "An argument is required" << std::endl;
    std::cout << "Run app --help or app help to display all available commands"
              << std::endl;
    return 1;
  }

  if (argc > 2) {
    std::cout << "Only 1 argument is expected" << std::endl;
  }

  struct mpd_connection *mpdConnection =
      mpd_connection_new("localhost", 6600, 10000);

  if (mpdConnection == NULL) {
    std::cerr << "Connection could not be initialized." << std::endl;
  }

  if (mpd_connection_get_error(mpdConnection) != MPD_ERROR_SUCCESS) {
    std::cerr << "A connection could not be made."
              << mpd_connection_get_error_message(mpdConnection) << std::endl;
    mpd_connection_free(mpdConnection);
    // handle_error(mpdConnection);
  }

  std::cout << "Connection success!" << std::endl;

  // TODO: initialize structs based on which command was called
  // so that status and song would be called if status is the
  // 1st argument, same with pause

  // create a status struct: needed to get server status
  struct mpd_status *status;
  mpd_send_status(mpdConnection);
  status = mpd_recv_status(mpdConnection);
  if (status == NULL) {
    handle_error(mpdConnection);
  }

  // create a song struct: needed to obtain song status
  struct mpd_song *song;
  mpd_send_current_song(mpdConnection);
  song = mpd_recv_song(mpdConnection);
  if (song == NULL) {
    handle_error(mpdConnection);
  }

  // Main logic of the client
  std::string command = argv[1];
  if (command == "restart" || command == "r") {
    play(mpdConnection);
  } else if (command == "toggle" || command == "t") {
    if (mpd_status_get_state(status) == MPD_STATE_PAUSE) {
      pause(mpdConnection, false);
    } else {
      pause(mpdConnection, true);
    }
  } else if (command == "skip" || command == "sk") {
    skip(mpdConnection);
  } else if (command == "help" || command == "--help") {
    displayHelpMenu();
  } else if (command == "status" || command == "s") {
    returnSong(mpdConnection, status, song);
  } else {
    std::cout << "Unknown argument \"" << command << "\""
              << "; see help menu using --help" << std ::endl;
    mpd_status_free(status);
    mpd_song_free(song);
    return 1;
  }

  // free the allocated memory
  // mpd_connection_free(mpdConnection);
  mpd_connection_free(mpdConnection);
  mpd_status_free(status);
  mpd_song_free(song);
  return 0;
}
