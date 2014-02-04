#include "audio_manager.h"

AudioManager::AudioManager() {
  gst_init(NULL, NULL);

  pipeline_ = gst_pipeline_new("audio-player");
  bus_      = gst_pipeline_get_bus(GST_PIPELINE(pipeline_));
  source_   = gst_element_factory_make("filesrc", "file-source");
  decoder_  = gst_element_factory_make("mad", "mp3-decoder");
  sink_     = gst_element_factory_make("autoaudiosink", "audio-output");

  gst_bin_add_many(GST_BIN(pipeline_), source_, decoder_, sink_, NULL);

  gst_element_link_many(source_, decoder_, sink_, NULL);

  gst_bus_add_signal_watch(bus_);
  g_signal_connect(bus_, "message", G_CALLBACK(AudioManager::GstBusCallback), NULL);

}

AudioManager::~AudioManager() {
  gst_element_set_state(pipeline_, GST_STATE_NULL);
  gst_object_unref(bus_);
  gst_object_unref(GST_OBJECT(pipeline_));
}

void AudioManager::PlayMp3File(const char* file) {
  g_object_set(G_OBJECT(source_), "location", file, NULL);
  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}

gboolean AudioManager::GstBusCallback(GstBus* bus, GstMessage* msg,
                                      gpointer data) {
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
      break;
    case GST_MESSAGE_ERROR: {
      gchar* debug;
      GError* error;
      gst_message_parse_error(msg, &error, &debug);
      g_free(debug);
      g_printerr("Error: %s\n", error->message);
      g_error_free(error);
      break;
    }
    default:
      break;
  }
  
  return TRUE;
}
