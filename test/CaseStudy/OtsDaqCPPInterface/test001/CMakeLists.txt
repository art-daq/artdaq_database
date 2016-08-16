include(CetTest)
cet_enable_asserts()

link_libraries(
  artdaq-database_OtsDaqCPPInterface
)

cet_test(DatabaseConfiguration_t USE_BOOST_UNIT)

install_headers()
install_source()

