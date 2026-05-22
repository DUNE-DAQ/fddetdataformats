
namespace dunedaq::fddetdataformats {

inline uint16_t TDEEthFrame::get_adc(int i_channel, int i_sample) const {

    // Note the generic get_adc_2d function takes the channel and the sample (timeslice) in reverse order
    // Also note that the value returned by get_adc_2d is stored in a uint64_t (TDEEthFrame::word_t) but is
    // guaranteed to be storable in a uint16_t since TDEEthFrame::s_bits_per_adc < 16. 
    
    return static_cast<uint16_t>( dunedaq::fddetdataformats::get_adc_2d<
				  TDEEthFrame::word_t,
				  TDEEthFrame::s_time_samples_per_frame,
				  TDEEthFrame::s_num_adc_words_per_ts,
				  TDEEthFrame::s_bits_per_adc>(
							       i_sample, i_channel,
							       adc_words
							       )
				  );
  }

  inline void TDEEthFrame::set_adc(int i_channel, int i_sample, uint16_t adc_val) {
    dunedaq::fddetdataformats::set_adc_2d<
      TDEEthFrame::word_t,
      TDEEthFrame::s_time_samples_per_frame,
      TDEEthFrame::s_num_adc_words_per_ts,
      TDEEthFrame::s_bits_per_adc>(
				   i_sample, i_channel,
				   adc_val,
				   adc_words
				   );

  }

} // namespace dunedaq::fddetdataformats
