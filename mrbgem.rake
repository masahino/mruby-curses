MRuby::Gem::Specification.new('mruby-curses') do |spec|
  spec.license = 'MIT'
  spec.authors = 'Kotaro Hibi'
  if build.kind_of?(MRuby::CrossBuild) && %w(x86_64-w64-mingw32 i686-w64-mingw32).include?(build.host_target)
    spec.cc.flags << '-DUSE_PDCURSES'
    spec.linker.libraries << 'curses'
  else
    spec.linker.libraries << 'ncurses'
  end
end
