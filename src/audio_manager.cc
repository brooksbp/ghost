#include "audio_manager.h"

#include <QtCore/QDebug>

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
  g_signal_connect(bus_, "message", G_CALLBACK(AudioManager::GstBusCallback), this);

  track_poller_ = new Timer(0, 100 * 1000 * 1000, true,
                            std::bind(&AudioManager::TrackPoller, this));
}

AudioManager::~AudioManager() {
  gst_element_set_state(pipeline_, GST_STATE_NULL);
  gst_object_unref(bus_);
  gst_object_unref(GST_OBJECT(pipeline_));
}

void AudioManager::PlayMp3File(const char* file) {
  qDebug() << "enter";
  gst_element_set_state(pipeline_, GST_STATE_READY);

  g_object_set(G_OBJECT(source_), "location", file, NULL);

  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
  playing_ = 1;
  track_poller_->Start();
  qDebug() << "exit";
}

void AudioManager::TrackPoller() {
  if (playing_) {
    gst_element_query_position(pipeline_, GST_FORMAT_TIME, &pos_);
    gst_element_query_duration(pipeline_, GST_FORMAT_TIME, &len_);
    PlaybackProgressCallback(pos_, len_);
  } else {
    track_poller_->Stop();
  }
}

gboolean AudioManager::GstBusCallback(GstBus* bus, GstMessage* msg,
                                      gpointer data) {
  AudioManager* this_ = static_cast<AudioManager*>(data);
  
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
      this_->playing_ = 0;
      this_->track_poller_->Stop();

      this_->eosCallback();
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
