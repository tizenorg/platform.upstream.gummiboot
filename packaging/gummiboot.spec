Name:           gummiboot
Version:        28
Release:        1
License:        LGPL-2.1
Summary:        EFI Kernel loader
Url:            http://cgit.freedesktop.org/gummiboot/
Group:          System/Boot
Source0:        http://cgit.freedesktop.org/%{name}/snapshot/%{name}-%{version}.tar.gz
Source1001:     packaging/gummiboot.manifest
BuildRequires:  gnu-efi
BuildRequires:  pkgconfig(blkid)
BuildRoot:      %{_tmppath}/%{name}-%{version}-build


%description
Gummiboot is a simple EFI bootloader.

%prep
%setup -q -n %{name}-%{version}

%build
cp %{SOURCE1001} .
%reconfigure
make

%install
rm -fr %{buildroot}
%make_install

%files
%defattr(-,root,root)
%manifest gummiboot.manifest
%{_bindir}/gummiboot
%{_prefix}/lib/gummiboot/gummibootx64.efi
