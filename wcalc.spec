%define name Wcalc
%define version 1.5.1

Summary: A flexible command-line calculator
Name: %{name}
Version: %{version}
Release: 1
Group: Applications
Url: http://homepage.mac.com/memoryhole/wcalc
Source: http://homepage.mac.com/memoryhole/wcalc/Wcalc-%{version}.tar.bz2
Copyright: GPL
Vendor: Kyle Wheeler <memoryhole@mac.com>
Packager: Kyle Wheeler <memoryhole@mac.com>

%description
Wcalc is a command-line calculator designed to accpet all valid mathematical expressions. It supports all standard mathematical operations, parenthesis, brackets, braces, trigonometric functions, hyperbolic trig functions, logs, and most boolean operators.

%prep
%setup -q

%build
make distro

%install
make install

%clean
[ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%files
%defattr(-,root,root)
/usr/local/bin/wcalc
%doc /usr/share/man/man1/wcalc.1*

%changelog
* Tue Jan 22 2002 Kyle Wheeler <memoryhole@mac.com>
 - Initial spec file.
