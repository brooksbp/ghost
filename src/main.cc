#include "base/command_line.h"
#include "base/file_path.h"

#include "track.h"
#include "library.h"

#include <gst/gst.h>
#include <glib.h>

#include <QtWidgets>

#include <iostream>


static gboolean bus_call(GstBus* bus, GstMessage* msg, gpointer data) {
  QApplication* app = (QApplication*) data;

  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
      app->quit();
      break;
    case GST_MESSAGE_ERROR: {
      gchar* debug;
      GError* error;

      gst_message_parse_error(msg, &error, &debug);
      g_free(debug);

      g_printerr("Error: %s\n", error->message);
      g_error_free(error);

      app->quit();
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


  QApplication app(argc, NULL);
  QWidget window;
  window.setWindowTitle("Ghost");
  window.show();

  
  Library library(dir);
  library.PrintTracks();


  gst_init(NULL, NULL);

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

  gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);

  gst_element_link_many(source, decoder, sink, NULL);


  GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  gst_bus_add_signal_watch(bus);
  g_signal_connect(bus, "message", G_CALLBACK(bus_call), &app);
  gst_object_unref(bus);

  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  // gst_element_set_state(pipeline, GST_STATE_NULL);
  // gst_object_unref(GST_OBJECT(pipeline));
  
  return app.exec();  
}
