
namespace dunedaq::fddetdataformats {

inline std::ostream&
operator<<(std::ostream& o, TDEHeader const& tde_header)
{
  return o << std::hex << "channel: " << tde_header.channel << "version: " << tde_header.version
           << "TAItime: " << tde_header.TAItime << " tde_header: " << tde_header.tde_header
           << " tde_errors: " << tde_header.tde_errors << std::dec << '\n';
}

inline std::ostream&
operator<<(std::ostream& o, Sample const& sampleinfo)
{
  return o << "sample: " << unsigned(sampleinfo.sample) << " reserved: " << unsigned(sampleinfo.reserved) << '\n';
}

inline std::ostream&
operator<<(std::ostream& o, TDE16Frame const& tde16frame)
{
  o << "Printing frame:" << '\n';
  o << tde16frame.m_daq_header << '\n';
  o << tde16frame.m_tde16_header << '\n';
  return o;
}

} // namespace dunedaq::fddetdataformats
