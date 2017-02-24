include(CetTest)
cet_enable_asserts()

link_libraries(
  artdaq-database_OtsDaqCPPInterface
)

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}f")
#cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}f")
cet_test(DatabaseConfiguration_t USE_BOOST_UNIT)

install_headers()
install_source()

