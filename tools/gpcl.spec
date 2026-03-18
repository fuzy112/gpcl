#
# gpcl.spec
# ~~~~~~~~~
#
# Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

Name:           gpcl
Version:        0.9.0
Release:        0
Summary:        General Purpose C++ Libraries
License:        BSL-1.0
URL:            https://github.com/fuzy112/gpcl
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
Requires:	%{libgpcl}

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
