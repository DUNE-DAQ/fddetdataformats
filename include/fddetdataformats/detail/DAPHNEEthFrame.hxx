
namespace dunedaq::fddetdataformats {

inline uint16_t
DAPHNEEthFrame::get_adc(int i) const
{

  // We're static_casting the returned DAPHNEEthFrame::word_t to a uint16_t, which is fine since
  // the ADC value is guaranteed to be storable in 16 bits

  return static_cast<uint16_t>(dunedaq::fddetdataformats::get_adc_1d<DAPHNEEthFrame::word_t,
                                                                     DAPHNEEthFrame::s_num_adc_words,
                                                                     DAPHNEEthFrame::s_bits_per_adc>(i, m_adc_words));
}

inline void
DAPHNEEthFrame::set_adc(int i, uint16_t val)
{ // NOLINT

  dunedaq::fddetdataformats::set_adc_1d<DAPHNEEthFrame::word_t,
                                        DAPHNEEthFrame::s_num_adc_words,
                                        DAPHNEEthFrame::s_bits_per_adc>(i, val, m_adc_words);
}

} // namespace dunedaq::fddetdataformats
