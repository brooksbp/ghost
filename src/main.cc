#include "base/command_line.h"
#include "base/file_path.h"

#include "track.h"
#include "library.h"

#include <gst/gst.h>
#include <glib.h>

#include <iostream>


static gboolean bus_call(GstBus* bus, GstMessage* msg, gpointer data) {
  GMainLoop* loop = (GMainLoop*) data;

  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
      g_main_loop_quit(loop);
      break;
    case GST_MESSAGE_ERROR: {
      gchar* debug;
      GError* error;

      gst_message_parse_error(msg, &error, &debug);
      g_free(debug);

      g_printerr("Error: %s\n", error->message);
      g_error_free(error);

      g_main_loop_quit(loop);
      break;
    }
    default:
      break;
  }
  
  return TRUE;
}


int main(int argc, const char* argv[]) {
  CommandLine cl(argc, argv);

  base::FilePath dir("../test-data/");
  if (cl.HasSwitch("dir")) {
    dir = cl.GetSwitchValuePath("dir");
  }

  Library library(dir);
  library.PrintTracks();

  
  int zero = 0;
  gst_init(&zero, NULL);

  GMainLoop* loop = g_main_loop_new(NULL, FALSE);

  GstElement* pipeline = gst_pipeline_new("audio-player");
  GstElement* source   = gst_element_factory_make("filesrc", "file-source");
  GstElement* decoder  = gst_element_factory_make("mad", "mp3-decoder");
  GstElement* sink     = gst_element_factory_make("autoaudiosink", "audio-output");

  if (!pipeline || !source || !decoder || !sink) {
    std::cout << "element failed" << std::endl;
    return 0;
  }

  std::cout << "location = " << library.GetATrack()->file_path_.value().c_str() << std::endl;
  g_object_set(G_OBJECT(source), "location", library.GetATrack()->file_path_.value().c_str(), NULL);

  GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  guint bus_watch_id = gst_bus_add_watch(bus, bus_call, loop);
  gst_object_unref(bus);

  gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);

  gst_element_link_many(source, decoder, sink, NULL);

  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  g_main_loop_run(loop);

  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT(pipeline));
  g_source_remove(bus_watch_id);
  g_main_loop_unref(loop);

}
