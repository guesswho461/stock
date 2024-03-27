"use client";

import * as React from 'react';
import Table from '@mui/material/Table';
import TableBody from '@mui/material/TableBody';
import TableCell from '@mui/material/TableCell';
import TableContainer from '@mui/material/TableContainer';
import TableHead from '@mui/material/TableHead';
import TableRow from '@mui/material/TableRow';
import Paper from '@mui/material/Paper';
import axios from "axios";
import { useEffect, useState } from 'react';
import { AdapterDayjs } from '@mui/x-date-pickers/AdapterDayjs';
import { LocalizationProvider } from '@mui/x-date-pickers/LocalizationProvider';
import { DatePicker } from '@mui/x-date-pickers/DatePicker';
import dayjs from 'dayjs';
import Box from '@mui/material/Box';
import Backdrop from '@mui/material/Backdrop';
import CircularProgress from '@mui/material/CircularProgress';

const getStockData = async (date, first) => {
    console.log("data from server")
    try {
        const response = await axios.post('http://localhost:8080/MaxQuantity', {
            date: date.format("YYYYMMDD"),
            first: first
        });
        console.log(response);
        return response?.data;
    } catch (error) {
        console.error(error);
    }
}

export default function BasicTable() {
    const [data, setData] = useState([]);
    const [date, setDate] = useState(dayjs());
    const [first, setFirst] = useState("50");
    const [isLoading, setIsLoading] = useState(true);

    useEffect(() => {
        handleDateOnChange(date, first)
    }, []);

    const handleDateOnChange = async (date, first) => {
        setIsLoading(true)
        const newData = await getStockData(date, first)
        setData(newData)
        setIsLoading(false)
    }

    return (
        <Box sx={{
            my: 4,
            display: 'flex',
            flexDirection: 'column',
            justifyContent: 'center',
            alignItems: 'center',
        }}
        >
            <Backdrop
                sx={{ color: '#fff', zIndex: (theme) => theme.zIndex.drawer + 1 }}
                open={isLoading}
            >
                <CircularProgress color="inherit" />
            </Backdrop>
            <LocalizationProvider dateAdapter={AdapterDayjs}>
                <DatePicker
                    label="pick a date"
                    value={date}
                    onChange={(newDate) => {
                        setDate(newDate);
                        handleDateOnChange(newDate, first);
                    }}
                    referenceDate={date}
                />
            </LocalizationProvider>
            <TableContainer component={Paper}>
                <Table>
                    <TableHead>
                        <TableRow>
                            <TableCell align="right">Date</TableCell>
                            <TableCell align="right">Index</TableCell>
                            <TableCell align="center">Name</TableCell>
                            <TableCell align="right">Quantity</TableCell>
                            <TableCell align="right">Open</TableCell>
                            <TableCell align="right">Close</TableCell>
                            <TableCell align="right">Highest</TableCell>
                            <TableCell align="right">Lowest</TableCell>
                        </TableRow>
                    </TableHead>
                    <TableBody>
                        {data.map((d) => (
                            <TableRow
                                key={d.tseINDEX}
                                sx={{ '&:last-child td, &:last-child th': { border: 0 } }}
                            >
                                <TableCell align="right">{d.tseDATE}</TableCell>
                                <TableCell align="right">{d.tseINDEX}</TableCell>
                                <TableCell align="center">{d.tseNAME}</TableCell>
                                <TableCell align="right">{d.tseQUANTITY}</TableCell>
                                <TableCell align="right">{d.tseOPEN}</TableCell>
                                <TableCell align="right">{d.tseCLOSE}</TableCell>
                                <TableCell align="right">{d.tseHIGHEST}</TableCell>
                                <TableCell align="right">{d.tseLOWEST}</TableCell>
                            </TableRow>
                        ))}
                    </TableBody>
                </Table>
            </TableContainer>
        </Box>
    );
}
