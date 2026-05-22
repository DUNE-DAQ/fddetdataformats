
namespace dunedaq::fddetdataformats {

inline uint8_t
DAPHNEEthStreamFrame::get_channel(const uint i_channel) const
{
  if (i_channel >= s_num_channels) {
    throw std::out_of_range(
      std::format("Requested channel index of {} is outside of allowed range 0-{}", i_channel, s_num_channels - 1));
  }

  return header.channel_words[i_channel].channel; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}

inline void
DAPHNEEthStreamFrame::set_channel(const uint i_channel, const uint8_t new_channel_val)
{
  if (i_channel >= s_num_channels) {
    throw std::out_of_range(
      std::format("Requested channel index of {} is outside of allowed range 0-{}", i_channel, s_num_channels - 1));
  }

  header.channel_words[i_channel].channel =
    new_channel_val; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}

inline uint16_t
DAPHNEEthStreamFrame::get_adc(uint i_adc, uint i_channel) const
{
  return static_cast<uint16_t>(
    dunedaq::fddetdataformats::
      get_adc_2d_as_1d<word_t, s_num_adc_words, s_bits_per_adc, s_adcs_per_channel, s_num_channels>(
        static_cast<int>(i_adc), static_cast<int>(i_channel), adc_words));
}

inline void
DAPHNEEthStreamFrame::set_adc(uint i_channel, uint i_adc, uint16_t val)
{ // NOLINT
  dunedaq::fddetdataformats::
    set_adc_2d_as_1d<word_t, s_num_adc_words, s_bits_per_adc, s_adcs_per_channel, s_num_channels>(
      static_cast<int>(i_adc), static_cast<int>(i_channel), val, adc_words);
}

} // namespace dunedaq::fddetdataformats
