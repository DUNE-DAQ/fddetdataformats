
namespace dunedaq::fddetdataformats {

inline uint16_t
DAPHNEStreamFrame::get_adc(int i_adc, int i_channel) const
{
  return static_cast<uint16_t>(
    dunedaq::fddetdataformats::
      get_adc_2d_as_1d<word_t, s_num_adc_words, s_bits_per_adc, s_adcs_per_channel, s_channels_per_frame>(
        i_adc, i_channel, adc_words));
}

inline void
DAPHNEStreamFrame::set_adc(int i_adc, int i_channel, uint16_t val)
{
  dunedaq::fddetdataformats::
    set_adc_2d_as_1d<word_t, s_num_adc_words, s_bits_per_adc, s_adcs_per_channel, s_channels_per_frame>(
      i_adc, i_channel, val, adc_words);
}

} // namespace dunedaq::fddetdataformats
