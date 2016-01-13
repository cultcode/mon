Name:		liblog4c
Version:	1.0
Release:	1%{?dist}
Summary:	liblog4c for Titan

Group:		Development/Tools
License:	Commercial
URL:		http://portal.titan.imgo.tv/
Source0:	%{name}-%{version}.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

#BuildRequires:	
#Requires:	

%description


%prep
%setup -q


%build


%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/lib64/
cp liblog4c.so %{buildroot}/usr/lib64/


%post
echo "%{name}-%{version} installed into /usr/lib64"


%postun
echo "%{name}-%{version} uninstalled from /usr/lib64"


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%doc
/usr/lib64/liblog4c.so



%changelog

