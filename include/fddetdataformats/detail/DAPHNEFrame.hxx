
namespace dunedaq::fddetdataformats {

inline uint16_t
DAPHNEFrame::get_adc(int i) const // NOLINT
{
  // We can safely case from word_t to uint16_t as the ADC value can always be represented in 16 bits
  return static_cast<uint16_t>(
    dunedaq::fddetdataformats::get_adc_1d<word_t, s_num_adc_words, s_bits_per_adc>(i, adc_words));
}

inline void
DAPHNEFrame::set_adc(int i, uint16_t val) // NOLINT
{
  dunedaq::fddetdataformats::set_adc_1d<word_t, s_num_adc_words, s_bits_per_adc>(i, val, adc_words);
}

// --- Trailer Accessors (Manual Shift–Mask Extraction) ---

inline bool
DAPHNEFrame::PeakDescriptorData::is_found(int idx) const // idx index 0 to 4
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  const word_t* tw = as_words();
  // In odd word, Found is in bit 31.
  return static_cast<uint8_t>((tw[2 * idx] >> 31) & 0x1); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline void
DAPHNEFrame::PeakDescriptorData::set_found(uint8_t val, int idx)
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("peak index out of range (must be 0-4)");
  if (val > 1)
    throw std::out_of_range("Found value out of range (must be 0-1)");
  word_t* tw = as_words();
  tw[2 * idx] =
    (tw[2 * idx] & ~(1u << 31)) | ((val & 0x1) << 31); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline uint32_t
DAPHNEFrame::PeakDescriptorData::get_adc_integral(int idx) const
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  const word_t* tw = as_words();
  return (tw[2 * idx] >> 8) & 0x7FFFFF; // Mask 23 bits // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline void
DAPHNEFrame::PeakDescriptorData::set_adc_integral(uint32_t val, int idx)
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  if (val > 0x7FFFFF)
    throw std::out_of_range("ADC_Integral value out of range (must be 0-8388607)");
  word_t* tw = as_words();
  tw[2 * idx] = (tw[2 * idx] & ~(0x7FFFFFu << 8)) |
                ((val & 0x7FFFFF) << 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline uint8_t
DAPHNEFrame::PeakDescriptorData::get_num_subpeaks(int idx) const
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  const word_t* tw = as_words();
  return static_cast<uint8_t>(tw[2 * idx] & 0xF); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline void
DAPHNEFrame::PeakDescriptorData::set_num_subpeaks(uint8_t val, int idx)
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  if (val > 0xF)
    throw std::out_of_range("Num_SubPeaks value out of range (must be 0-15)");
  word_t* tw = as_words();
  tw[2 * idx] = (tw[2 * idx] & ~0xF) | (val & 0xF); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline uint16_t
DAPHNEFrame::PeakDescriptorData::get_samples_over_baseline(int idx) const
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  const word_t* tw = as_words();
  return static_cast<uint16_t>((tw[2 * idx + 1] >> 23) &
                               0x1FF); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline void
DAPHNEFrame::PeakDescriptorData::set_samples_over_baseline(uint16_t val, int idx)
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  if (val > 0x1FF)
    throw std::out_of_range("Time_Over_Baseline value out of range (must be 0-511)");
  word_t* tw = as_words();
  tw[2 * idx + 1] = (tw[2 * idx + 1] & ~(0x1FFu << 23)) |
                    ((val & 0x1FF) << 23); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline uint16_t
DAPHNEFrame::PeakDescriptorData::get_sample_max(int idx) const
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  const word_t* tw = as_words();
  return static_cast<uint16_t>((tw[2 * idx + 1] >> 14) &
                               0x1FF); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline void
DAPHNEFrame::PeakDescriptorData::set_sample_max(uint16_t val, int idx)
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  if (val > 0x1FF)
    throw std::out_of_range("Time_Peak value out of range (must be 0-511)");
  word_t* tw = as_words();
  tw[2 * idx + 1] = (tw[2 * idx + 1] & ~(0x1FFu << 14)) |
                    ((val & 0x1FF) << 14); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline uint16_t
DAPHNEFrame::PeakDescriptorData::get_adc_max(int idx) const
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  const word_t* tw = as_words();
  // Even word for idx is at index 2*idx+1; ADC Max is in bits 13:0.
  return static_cast<uint16_t>(tw[2 * idx + 1] & 0x3FFF); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline void
DAPHNEFrame::PeakDescriptorData::set_adc_max(uint16_t val, int idx)
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  if (val > 0x3FFF)
    throw std::out_of_range("ADC Max value out of range (must be 0-16383)");
  word_t* tw = as_words();
  tw[2 * idx + 1] =
    (tw[2 * idx + 1] & ~0x3FFFu) | (val & 0x3FFF); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

inline uint16_t
DAPHNEFrame::PeakDescriptorData::get_sample_start(int idx) const
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Time_Start index out of range (must be 0-4)");

  const word_t* tw = as_words();
  if (idx < 3) {
    int shift = 22 - 10 * idx;
    return static_cast<uint16_t>((tw[10] >> shift) & 0x3FF); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  } else {
    int shift = 22 - 10 * (idx - 3);
    return static_cast<uint16_t>((tw[11] >> shift) & 0x3FF); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }
}

inline void
DAPHNEFrame::PeakDescriptorData::set_sample_start(uint16_t val, int idx)
{
  if (idx < 0 || idx > 4)
    throw std::out_of_range("Time_Start index out of range (must be 0–4)");
  if (val > 0x3FF)
    throw std::out_of_range("Time_Start value out of range (must be 0–1023)");

  word_t* tw = as_words();
  word_t mask = 0x3FFu;

  if (idx < 3) {
    int shift = 22 - 10 * idx;
    tw[10] =
      (tw[10] & ~(mask << shift)) | ((val & mask) << shift); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  } else {
    int shift = 22 - 10 * (idx - 3);
    tw[11] =
      (tw[11] & ~(mask << shift)) | ((val & mask) << shift); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }
}

} // namespace dunedaq::fddetdataformats
