%define name Wcalc
%define version 1.5.2

Summary: A flexible command-line calculator
Name: %{name}
Version: %{version}
Release: 1
Group: Applications
Url: http://w-calc.sf.net/
Source: http://prdownloads.sf.net/w-calc/Wcalc-%{version}.tar.bz2
License: GPL
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Vendor: Kyle Wheeler <kyle-rpm@memoryhole.ncr.nd.edu>
Packager: Kyle Wheeler <kyle-rpm@memoryhole.ncr.nd.edu>

%description
Wcalc is a command-line calculator designed to accept all valid mathematical expressions. It supports all standard mathematical operations, parenthesis, brackets, braces, trigonometric functions, hyperbolic trig functions, logs, and most boolean operators.

%prep
%setup -q

%build
%configure
make

%install
[ -d $RPM_BUILD_ROOT ] && rm -rf ${RPM_BUILD_ROOT}
make DESTDIR=$RPM_BUILD_ROOT install

%clean
[ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%files
%defattr(-,root,root)
%doc ChangeLog COPY* NEWS README
%{_bindir}/*
%{_mandir}/*/*

%changelog
* Wed Jan 29 2003 Kyle Wheeler <kyle-rpm@memoryhole.ncr.nd.edu>
 - A few minor modifications
* Wed Jan 29 2003 Mark Schreiber <mark7@andrew.cmu.edu>
 - Fixed spec file to build on Red Hat
* Mon Jan 27 2003 Kyle Wheeler <kyle-rpm@memoryhole.ncr.nd.edu>
 - Fixed spec file for new URL
 - Fixed spec file to use automake/autoconf
* Tue Jan 22 2002 Kyle Wheeler <memoryhole@mac.com>
 - Initial spec file.
