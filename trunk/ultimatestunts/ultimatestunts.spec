# Ultimate Stunts RPM spec file.

Summary: Ultimate Stunts is a remake of the famous DOS game 'stunts'.
Name: ultimatestunts
Version: 047
Release: 0.fdr.1
License: GPL
Group: Amusements/Games
Source0: ftp://ftp.sourceforge.net/pub/sourceforge/u/ul/%{name}/%{name}-src-%{version}.tar.gz
Source1: ftp://ftp.sourceforge.net/pub/sourceforge/u/ul/%{name}/%{name}-data-%{version}.tar.gz
URL: http://ultimatestunts.sf.net/
Vendor: CJP <cornware-cjp@users.sourceforge.net>
Packager: Neil Zanella <nzanella@users.sourceforge.net>
BuildRoot: %{_tmppath}/%{name}-%{version}-root
BuildRequires: gawk make gcc-c++ binutils SDL-devel >= 1.1.0 xorg-x11-devel ode fmod
Requires: SDL >= 1.1.0 ode fmod

%description
Ultimate Stunts is a remake of the famous DOS game 'stunts', a 3D racing game
with simple CGA/EGA/VGA graphics and no texture or smooth shading. Spectacular
stunts with loopings and bridges to jump over make the game fun to play. One
of the best aspects of Ultimate Stunts is the track editor. Because of the
tile-based tracks, every games is able to make their own tracks.

%prep
%setup -n %{name}-src-%{version}

%build
%{configure}
#%{__sed} 's/soundlibs .*/soundlibs = -lfmod/' Makefile > Makefile.new
#%{__mv} -f Makefile.new Makefile
%{__make}

%install
%{__rm} -rf "%{buildroot}"
%{makeinstall}
%{__mkdir} -p "%{buildroot}"/%{_datadir}/%{name}
%{__tar} -C "%{buildroot}"/%{_datadir}/%{name} -zxvf %{_sourcedir}/%{name}-data-%{version}.tar.gz

%clean
%{__rm} -rf "%{buildroot}"

%files
%defattr(-, root, root)
%{_bindir}/stunts3dedit
%{_bindir}/stuntsai
%{_bindir}/stuntsserver
%{_bindir}/trackedit
%{_bindir}/ultimatestunts
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/*

%changelog
* Wed Oct 07 2004 Neil Zanella <nzanella@users.sourceforge.net>
- Initial release.
