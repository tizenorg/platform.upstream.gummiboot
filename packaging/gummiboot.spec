Name:           gummiboot
Version:        48
Release:        0
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

%package splash
Summary:	Gummiboot default splash image
BuildArch: 	noarch
Requires:	%{name} = %{version}

%description splash
This package contains the Gummiboot default splash image. It's the
project logo and maintained in the upstream gummiboot Git tree.

%prep
%setup -q -n %{name}-%{version}

%build
cp %{SOURCE1001} .
%reconfigure --with-efi-libdir=/usr/lib --with-efi-ldsdir=/usr/lib
%__make

%install
rm -fr %{buildroot}
%make_install

mkdir -p %{buildroot}/%{_datadir}/%{name}
cp test/splash.bmp %{buildroot}/%{_datadir}/%{name}

%files
%manifest %{name}.manifest
%defattr(-,root,root)
%manifest gummiboot.manifest
%{_bindir}/gummiboot
%{_prefix}/lib/%{name}/gummiboot*.efi
%{_prefix}/lib/%{name}/linux*.efi.stub

%files splash
%{_datadir}/%{name}/splash.bmp
