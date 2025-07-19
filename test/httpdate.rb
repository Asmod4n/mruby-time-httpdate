assert('Time#httpdate produces correct RFC1123 string') do
  httpdate = Time.at(0).httpdate
  assert_equal 'Thu, 01 Jan 1970 00:00:00 GMT', httpdate
end
