%define name Wcalc
%define version 1.5.2

Summary: A flexible command-line calculator
Name: %{name}
Version: %{version}
Release: 1
Group: Applications
Url: http://w-calc.sf.net/
Source: http://prdownloads.sf.net/w-calc/Wcalc-%{version}.tar.bz2
Copyright: GPL
Vendor: Kyle Wheeler <kyle-rpm@memoryhole.ncr.nd.edu>
Packager: Kyle Wheeler <kyle-rpm@memoryhole.ncr.nd.edu>

%description
Wcalc is a command-line calculator designed to accpet all valid mathematical expressions. It supports all standard mathematical operations, parenthesis, brackets, braces, trigonometric functions, hyperbolic trig functions, logs, and most boolean operators.

%prep
%setup -q

%build
./configure --prefix=/usr
make

%install
make install

%clean
[ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%files
%defattr(-,root,root)
/usr/local/bin/wcalc
%doc /usr/share/man/man1/wcalc.1*

%changelog
* Mon Jan 27 2003 Kyle Wheeler <kyle-rpm@memoryhole.ncr.nd.edu>
 - Fixed spec file for new URL
 - Fixed spec file to use automake/autoconf
* Tue Jan 22 2002 Kyle Wheeler <memoryhole@mac.com>
 - Initial spec file.
