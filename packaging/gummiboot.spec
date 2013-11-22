Name:           gummiboot
Version:        36
Release:        2
License:        LGPL-2.1
Summary:        EFI Kernel loader
Url:            http://cgit.freedesktop.org/gummiboot/
Group:          Base/Startup
Source0:        http://cgit.freedesktop.org/%{name}/snapshot/%{name}-%{version}.tar.gz
Source1001:     gummiboot.manifest
BuildRequires:  gnu-efi
BuildRequires:  pkgconfig(blkid)
ExclusiveArch:  %ix86 x86_64


%description
Gummiboot is a simple EFI bootloader.

%prep
%setup -q -n %{name}-%{version}

%build
cp %{SOURCE1001} .
%reconfigure \
	--with-efi-libdir=/usr/lib \
	--with-efi-ldsdir=/usr/lib \
	--enable-gpo
make

%install
rm -fr %{buildroot}
%make_install

%files
%manifest %{name}.manifest
%defattr(-,root,root)
%manifest gummiboot.manifest
%{_bindir}/gummiboot
%{_bindir}/ppm2bgrx
%{_datadir}/gummiboot/gummiboot.ppm
%ifarch x86_64
%{_prefix}/lib/gummiboot/gummibootx64.efi
%else
%{_prefix}/lib/gummiboot/gummibootia32.efi
%endif
