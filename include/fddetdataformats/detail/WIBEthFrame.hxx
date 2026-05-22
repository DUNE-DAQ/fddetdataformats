
namespace dunedaq::fddetdataformats {

inline uint16_t
WIBEthFrame::get_adc(int i_channel, int i_sample) const
{

  // Note the generic get_adc_2d function takes the channel and the time sample in reverse order
  return static_cast<uint16_t>(
    dunedaq::fddetdataformats::get_adc_2d<word_t, s_time_samples_per_frame, s_num_adc_words_per_ts, s_bits_per_adc>(
      i_sample, i_channel, adc_words));
}

inline void
WIBEthFrame::set_adc(int i_channel, int i_sample, uint16_t adc_val)
{

  // Note the generic set_adc_2d function takes the channel and the time sample in reverse order
  dunedaq::fddetdataformats::set_adc_2d<word_t, s_time_samples_per_frame, s_num_adc_words_per_ts, s_bits_per_adc>(
    i_sample, i_channel, adc_val, adc_words);
}

} // namespace dunedaq::fddetdataformats
