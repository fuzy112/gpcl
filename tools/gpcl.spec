#
# spec file for package gpcl
#
# Copyright (c) 2022 SUSE LLC
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#


Name:           gpcl
Version:        0.5.0
Release:        0
Summary:        General Purpose C++ Libraries
License:        BSL-1.0
URL:            https://github.com/chingyat/gpcl
Source0:        gpcl.tar.gz
Source1:	html_book_20190607.tar.xz
BuildRequires:  c++_compiler cmake
# Requires:       

%description
A C++ 17 library.

%define		libgpcl lib%{name}0

%package	-n %{libgpcl}
Summary:	General Purpose C++ Libraries

%description	-n %{libgpcl}
A C++ 17 library.

%package	dev
Summary:	Development files for GPCL.
Requires:	%{name}

%description	dev
Development files for GPCL.

%prep
%setup -q -n %{name}
mkdir data
tar xf %{SOURCE1} -C data

%build
%cmake \
  -DGPCL_BUILD_TESTING=OFF \
  -DFETCHCONTENT_SOURCE_DIR_CPPREFERENCE=%{_builddir}/gpcl/data
make %{?_smp_mflags} all doc

%install
%make_install -C build
rm -f %{buildroot}/%{_libdir}/libgpcl.so.500

%post -n %{libgpcl}
/sbin/ldconfig -p

%files -n %{libgpcl}
%license LICENSE_1_0.txt
%doc README.md
%{_libdir}/libgpcl.so.%{version}

%files dev
%{_includedir}/gpcl.hpp
%{_includedir}/gpcl/
%{_libdir}/cmake/gpcl/
%{_libdir}/libgpcl.so

%changelog
